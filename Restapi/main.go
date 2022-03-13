package main

import (
	"fmt"

	"github.com/kd993595/restapi/httphandler"
)

func main() {
	fmt.Println("Rest API v2.0 - Mux Routers")
	fmt.Println("http://127.0.0.1:8000")
	httphandler.HandleRequests()
	//dbinterface.ReadTable()
}
