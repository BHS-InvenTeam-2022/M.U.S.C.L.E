package httphandler

import (
	"bytes"
	"database/sql"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"strings"

	"github.com/gorilla/handlers"
	"github.com/gorilla/mux"
	"github.com/kd993595/restapi/dbinterface"
	"golang.org/x/crypto/bcrypt"
)

var db *sql.DB
var db2 *sql.DB
var pepper = "myownpepper"

type HttpMessage struct {
	Code int    `json:"code"`
	Msg  string `json:"message"`
}

//w.WriteHeader(int)  used to write the status code have to add to all endpoints

func homePage(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "Welcome to the HomePage!")
	fmt.Println("Endpoint Hit: homePage")
}

//hashes password and returns base64 encoding
func HashPassword(password string, salt string) (string, error) {
	bytes, err := bcrypt.GenerateFromPassword([]byte(pepper+salt+password), 14)
	if err != nil {
		log.Fatal(err)
	}
	encodedText := base64.StdEncoding.EncodeToString([]byte(bytes))
	dbpass := fmt.Sprintf("%s$%s$%s", "bcrypt", salt, encodedText)
	return dbpass, err
}

//checks if hash and password are the same returns true if they are
func CheckPasswordHash(password, hash string) bool {
	parts := strings.Split(hash, "$")
	functionname := parts[0]
	_ = functionname
	salt := parts[1]
	hashpassword := parts[2]
	rawDecodedText, err := base64.StdEncoding.DecodeString(hashpassword)
	if err != nil {
		log.Fatal(err)
	}
	err = bcrypt.CompareHashAndPassword([]byte(rawDecodedText), []byte(pepper+salt+password))
	return err == nil
}

func userInfo(w http.ResponseWriter, r *http.Request) {

	/*vars := mux.Vars(r) //use this for paramters stored in url like /{id}
	fmt.Println(vars)
	urlParams := r.URL.Query() //use this for query paramters stored in map
	fmt.Println(urlParams)*/

	urlParams := r.URL.Query()

	if len(urlParams["username"]) == 0 {
		w.WriteHeader(400)
		errormsg := HttpMessage{Code: 400, Msg: "username must be provided"}
		json.NewEncoder(w).Encode(errormsg)
		return
	} else if len(urlParams["password"]) == 0 {
		w.WriteHeader(400)
		errormsg := HttpMessage{Code: 400, Msg: "password must be provided"}
		json.NewEncoder(w).Encode(errormsg)
		return
	}
	username := urlParams["username"][0]

	person := dbinterface.SearchForPerson(db, username)
	if person.Id == 0 {
		w.WriteHeader(404)
		errormsg := HttpMessage{Code: 404, Msg: "user doesn't exist"}
		json.NewEncoder(w).Encode(errormsg)
		return
	}
	if !CheckPasswordHash(urlParams["password"][0], person.Password) {
		w.WriteHeader(403)
		errormsg := HttpMessage{Code: 403, Msg: "password is incorrect"}
		json.NewEncoder(w).Encode(errormsg)
		return
	}

	w.WriteHeader(200)
	json.NewEncoder(w).Encode(person)
}
func userCreate(w http.ResponseWriter, r *http.Request) {
	reqBody, _ := ioutil.ReadAll(r.Body)

	var newUser dbinterface.Person
	err := json.Unmarshal(reqBody, &newUser)
	if err != nil {
		log.Fatalf("error in unmarshalling: %v", err)
	}
	valid, msg := newUser.CheckValid()
	if !valid {
		w.WriteHeader(406)
		errormsg := HttpMessage{Code: 406, Msg: msg}
		json.NewEncoder(w).Encode(errormsg)
		return
	}

	person := dbinterface.SearchForPerson(db, newUser.Username)
	if person.Id != 0 {
		w.WriteHeader(409)
		errormsg := HttpMessage{Code: 409, Msg: "user could not be created username already taken"}
		json.NewEncoder(w).Encode(errormsg)
		return
	}

	newUser.Password, _ = HashPassword(newUser.Password, newUser.Username)

	valid = dbinterface.AddPerson(db, newUser)
	if !valid {
		w.WriteHeader(409)
		errormsg := HttpMessage{Code: 409, Msg: "user could not be created"}
		json.NewEncoder(w).Encode(errormsg)
		return
	}
	json.NewEncoder(w).Encode(newUser)

}

func eggCreate(w http.ResponseWriter, r *http.Request) {
	reqBody, _ := ioutil.ReadAll(r.Body)
	eggrequest := struct {
		EggId string `json:"eggId"`
		Data  string `json:"data"`
	}{}

	reqBody = bytes.ReplaceAll(reqBody, []byte("\r"), []byte(""))

	err := json.Unmarshal(reqBody, &eggrequest)
	if err != nil {
		log.Fatalf("error in unmarshalling: %v", err)
	}

	dbinterface.AddRecords(db2, eggrequest.Data, eggrequest.EggId)
	w.WriteHeader(201)
	endmsg := HttpMessage{Code: 201, Msg: "records successfully added"}
	json.NewEncoder(w).Encode(endmsg)

}
func eggInfo(w http.ResponseWriter, r *http.Request) {
	urlParams := r.URL.Query()
	if len(urlParams["eggId"]) == 0 || len(urlParams["datatype"]) == 0 {
		errormsg := HttpMessage{Code: 400, Msg: "eggId and datatpe must be provided"}
		json.NewEncoder(w).Encode(errormsg)
		return
	}

	data, clock_data := dbinterface.ReadRecords(db2, urlParams["datatype"][0], urlParams["eggId"][0])
	datapacket := struct {
		D string `json:"data"`
		C string `json:"clock_data"`
	}{D: data, C: clock_data}

	fmt.Println("reading records")

	json.NewEncoder(w).Encode(datapacket)
}

func HandleRequests() {

	var err error
	db, err = sql.Open("sqlite3", "./database.db")
	if err != nil {
		log.Fatal(err)
	}

	defer db.Close()

	db2, err = sql.Open("sqlite3", "./database2.db")
	if err != nil {
		log.Fatal(err)
	}

	defer db2.Close()

	myRouter := mux.NewRouter().StrictSlash(true)

	headersOk := handlers.AllowedHeaders([]string{"X-Requested-With", "Content-Type"})
	originsOk := handlers.AllowedOrigins([]string{"*"})
	methodsOk := handlers.AllowedMethods([]string{"GET", "HEAD", "POST", "PUT", "OPTIONS"})

	myRouter.HandleFunc("/", homePage)
	myRouter.HandleFunc("/user", userCreate).Methods("POST")
	myRouter.HandleFunc("/user", userInfo)

	myRouter.HandleFunc("/egg", eggCreate).Methods("POST")
	myRouter.HandleFunc("/egg", eggInfo)

	//myRouter.HandleFunc("/article/{id}", userInfo)

	log.Fatal(http.ListenAndServe(":8000", handlers.CORS(originsOk, headersOk, methodsOk)(myRouter)))
}
