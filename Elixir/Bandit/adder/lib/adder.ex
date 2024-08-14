defmodule Adder do
  use Application

  def start(_type, _args) do
    children = [
      {Bandit, plug: Adder.Router, scheme: :http, options: [port: 4000]}
    ]

    opts = [strategy: :one_for_one, name: Adder.Supervisor]
    Supervisor.start_link(children, opts)
  end
end