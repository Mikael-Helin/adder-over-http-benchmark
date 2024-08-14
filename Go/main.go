package main

import (
	"fmt"
	"net/http"
	"strconv"
	"strings"
)

func adderHandler(w http.ResponseWriter, r *http.Request) {
	// Assuming the URL path is /num1/num2
	segments := r.URL.Path[1:] // Trim leading '/'
	numbers := strings.Split(segments, "/")
	num1, _ := strconv.Atoi(numbers[0])
	num2, _ := strconv.Atoi(numbers[1])
	result := num1 + num2
	fmt.Fprintf(w, "{\"result\": %d}", result)
}

func main() {
	http.HandleFunc("/", adderHandler)
	fmt.Println("Server is running on http://localhost:4000")
	http.ListenAndServe(":4000", nil)
}
