package main

import (
	"fmt"

	"github.com/kd993595/restapi/httphandler"
)

func main() {
	fmt.Println("Rest API v2.0 - Mux Routers")
	httphandler.HandleRequests()
	//dbinterface.ReadTable()
}
