module Prometo = Yawaramin__Prometo;
module ParseError = Decode.ParseError;
module Decode = Decode.AsResult.OfParseError.Pipeline;

module Header = {
  [@react.component]
  let make = () => {
    <div
      className="flex justify-center p-4 text-5xl text-white bg-gray-900 rounded">
      {React.string(" Todo List")}
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

[@react.component]
let make = () => {
  let ({todos}, dispatch) = React.useReducer(reducer, {todos: todos});

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

  <div className="p-4">
    <Header />
    {todos
     ->List.map(todo => <Todo todo dispatch key={todo.id->string_of_int} />)
     ->List.toArray
     ->React.array}
  </div>;
};
