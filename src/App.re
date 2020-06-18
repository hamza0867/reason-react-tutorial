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

module Todo = {
  [@react.component]
  let make = (~todo) => {
    <div className="flex items-center p-4 my-2 text-2xl bg-gray-200 rounded ">
      <input className="mr-4" type_="checkbox" checked={todo.completed} />
      <span> {React.string(todo.title)} </span>
    </div>;
  };
};

let todo = {id: 1, title: "Have fun", completed: false};

let todos = [
  {id: 1, title: "Learn React", completed: true},
  {id: 2, title: "Learn ReaonML", completed: false},
  {id: 3, title: "Have fun", completed: false},
];

[@react.component]
let make = () => {
  <div className="p-4">
    <Header />
    {todos->List.map(todo => <Todo todo />)->List.toArray->React.array}
  </div>;
};
