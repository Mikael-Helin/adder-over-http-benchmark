defmodule AdderTest do
  use ExUnit.Case
  doctest Adder

  test "greets the world" do
    assert Adder.hello() == :world
  end
end
