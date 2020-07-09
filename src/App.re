module Prometo = Yawaramin__Prometo;
module ParseError = Decode.ParseError;
module Decode = Decode.AsResult.OfParseError.Pipeline;

module Header = {
  [@react.component]
  let make = (~path: list(string)) => {
    <div
      className="flex flex-col items-center p-4 text-5xl text-white bg-gray-900 rounded">
      <div> {React.string(" Todo List")} </div>
      <div className="text-base">
        <span
          className={"cursor-pointer mr-2" ++ (path == [] ? " underline" : "")}
          onClick={_ => ReasonReactRouter.push("/")}>
          {React.string("Home")}
        </span>
        <span
          className={
            "cursor-pointer ml-2" ++ (path == ["about"] ? " underline" : "")
          }
          onClick={_ => ReasonReactRouter.push("/about")}>
          {React.string("About")}
        </span>
      </div>
    </div>;
  };
};

type todo = {
  id: int,
  title: string,
  completed: bool,
};

let make = (id, title, completed) => {id, title, completed};

let decode = json =>
  Decode.(
    succeed(make)
    |> field("id", intFromNumber)
    |> field("title", string)
    |> field("completed", boolean)
    |> run(json)
  );

let decodeTodos = Decode.list(decode);

let todos = [
  {id: 1, title: "Learn React", completed: true},
  {id: 2, title: "Learn ReaonML", completed: false},
  {id: 3, title: "Have fun", completed: false},
];

type state = {todos: list(todo)};

type action =
  | ToggleComplete(int)
  | Reset(list(todo));

let reducer = (state, action) =>
  switch (action) {
  | ToggleComplete(id) => {
      todos:
        state.todos
        ->List.map(todo =>
            if (id == todo.id) {
              {...todo, completed: !todo.completed};
            } else {
              todo;
            }
          ),
    }
  | Reset(todos) => {todos: todos}
  };

module Todo = {
  [@react.component]
  let make = (~todo, ~dispatch) => {
    <div className="flex items-center p-4 my-2 text-2xl bg-gray-200 rounded ">
      <input
        className="mr-4"
        type_="checkbox"
        checked={todo.completed}
        onChange={_ => dispatch(ToggleComplete(todo.id))}
      />
      <span> {React.string(todo.title)} </span>
    </div>;
  };
};

module BlueDiv = {
  [@bs.module "./JsComponent"] [@react.component]
  external make: (~children: React.element) => React.element = "BlueDiv";
};

[@bs.module "./JsComponent"] external add: (int, int) => int = "add";

[@react.component]
let make = () => {
  let ({todos}, dispatch) = React.useReducer(reducer, {todos: todos});

  let x = add(2, 3);

  Js.log(x);

  React.useEffect0(() => {
    let todosPromise =
      Fetch.fetch("http://localhost:8080/todos")->Prometo.fromPromise;
    todosPromise
    ->Prometo.thenPromise(~f=res => Fetch.Response.json(res))
    ->Prometo.forEach(~f=data => {
        Js.log(data);
        let maybeTodos = decodeTodos(data);
        switch (maybeTodos) {
        | Ok(todos) => dispatch(Reset(todos))
        | Error(e) => Js.Console.error(ParseError.failureToDebugString(e))
        };
      });

    // // input => promise(output) : thenPromise
    // fetch("remoteUrl").then(res => res.json())
    // // input => output : map
    // .then( int => int.toString() )
    // // input => () : forEach
    // .then( data => setState(data) )

    Some(() => Prometo.cancel(todosPromise));
  });

  let url = ReasonReactRouter.useUrl();

  <div className="p-4">
    <Header path={url.path} />
    {switch (url.path) {
     | [] =>
       todos
       ->List.map(todo => <Todo todo dispatch key={todo.id->string_of_int} />)
       ->List.toArray
       ->React.array
     | ["about"] =>
       <BlueDiv> {React.string("This is the about page")} </BlueDiv>
     | _ => React.string("Not found")
     }}
  </div>;
};
