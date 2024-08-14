num1 = 1000000000
num2 = 0
request_count = 0

function request()
    path = "/" .. num1 .. "/" .. num2
    req = wrk.format("GET", path)

    num1 = num1 - 1
    num2 = num2 + 1

    return req
end

function done(summary, latency, requests)
    print("Total requests made: " .. request_count)
end
