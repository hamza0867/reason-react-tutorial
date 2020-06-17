module Header = {
  [@react.component]
  let make = () => {
    <div
      className="flex justify-center p-4 text-5xl text-white bg-gray-900 rounded">
      {React.string(" Todo List")}
    </div>;
  };
};

[@react.component]
let make = () => {
  <div className="p-4"> <Header /> </div>;
};
