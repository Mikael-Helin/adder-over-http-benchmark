defmodule Adder.Router do
  use Plug.Router

  plug :match
  plug :dispatch

  get "/:num1/:num2" do
    num1 = String.to_integer(conn.params["num1"])
    num2 = String.to_integer(conn.params["num2"])
    result = num1 + num2

    send_resp(conn, 200, Integer.to_string(result))
  end

  match _ do
    send_resp(conn, 404, "Not Found")
  end
end