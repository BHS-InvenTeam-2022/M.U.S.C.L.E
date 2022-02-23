package dbinterface

import (
	"database/sql"
	"fmt"
	"log"
	"time"

	_ "github.com/mattn/go-sqlite3"
)

type Person struct {
	Id       int    `json:"Id"`
	Username string `json:"Username"`
	Password string `json:"Password"`
	Email    string `json:"Email"`
	Eggid    string `json:"EggId"`
	Eggnames string `json:"Eggnames"`
}

/*
checks if a person struct is completely filled and returns true with empty string if its completely filled
*/
func (P *Person) CheckValid() (bool, string) {
	if P.Username == "" {
		return false, "username missing"
	} else if P.Password == "" {
		return false, "password missing"
	} else if P.Email == "" {
		return false, "email missing"
	} else if P.Eggid == "" {
		return false, "eggid missing"
	} else if P.Eggnames == "" {
		return false, "eggnames missing"
	}
	return true, ""
}

/*
example function for testing purposes
*/
func ReadTable() {
	db, err := sql.Open("sqlite3", "./database2.db")
	if err != nil {
		log.Fatal(err)
	}

	defer db.Close()

	stmt, _ := db.Prepare(`CREATE TABLE IF NOT EXISTS egg_model (
		id INTEGER NOT NULL, 
		"eggId" VARCHAR(75) NOT NULL, 
		temp_data FLOAT NOT NULL, 
		ph_data FLOAT NOT NULL, 
		clock_data VARCHAR(25) NOT NULL, 
		salinity_data FLOAT NOT NULL, 
		PRIMARY KEY (id)
	)`)
	defer stmt.Close()
	stmt.Exec()

	start := time.Now()
	//AddRecords(db, data, "kev171")
	//res_d, res_c := ReadRecords(db, "temp", "kev171")
	//fmt.Println(res_d)
	//fmt.Println(res_c)
	duration := time.Since(start)
	log.Println(duration)

	/*newPerson := Person{
		Username: "juanito",
		Password: "reaper17",
		Email:    "beebee@gmail.com",
		Eggid:    "juanito1",
		Eggnames: "myprecious",
	}
	_ = newPerson

	searchString := "kev17"
	people := SearchForPerson(db, searchString)
	fmt.Println(people.Id)
	fmt.Println(people)

		//adding a Person to database
		addPerson(db, newPerson)

		//searching for a Person thorugh Username
		searchString := "juanito"
		people := searchForPerson(db, searchString)
		fmt.Println(people)

		//updating Person through Username and must get existing Person and change factors about it to update
		people[0].Eggnames = "yourprecious"
		affected := updatePerson(db, people[0])

		//deleting a Person from database through Username
		affected := deletePerson(db, searchString)
	*/

}

/*
Adds a record to the database taking in a person struct and returning a bool if created or not
*/
func AddPerson(db *sql.DB, newPerson Person) bool {

	stmt, _ := db.Prepare("INSERT INTO user_model (Username, Password, Email, Eggid, Eggnames) VALUES (?, ?, ?, ?, ?)")
	_, err := stmt.Exec(newPerson.Username, newPerson.Password, newPerson.Email, newPerson.Eggid, newPerson.Eggnames)
	defer stmt.Close()

	if err != nil {
		fmt.Println(err)
		return false
	}

	fmt.Printf("Added %v with Password: %v \n", newPerson.Username, newPerson.Password)
	return true
}

/*
searches for person in database given username string and returns a struct of the person
*/
func SearchForPerson(db *sql.DB, searchString string) Person {

	rows, err := db.Query("SELECT Id, Username, Password, Email, Eggid, Eggnames FROM user_model WHERE Username = '" + searchString + "' LIMIT 1")

	if err != nil {
		log.Fatal(err)
	}
	err = rows.Err()
	if err != nil {
		log.Fatal(err)
	}
	defer rows.Close()

	people := make([]Person, 0)

	for rows.Next() {
		ourPerson := Person{}
		err = rows.Scan(&ourPerson.Id, &ourPerson.Username, &ourPerson.Password, &ourPerson.Email, &ourPerson.Eggid, &ourPerson.Eggnames)
		if err != nil {
			log.Fatal(err)
		}

		people = append(people, ourPerson)
	}

	if len(people) == 0 {
		return Person{}
	}

	return people[0]
}

/*
updates record of person in User databse given another person struct and matches based on username string
*/
func UpdatePerson(db *sql.DB, ourPerson Person) int64 {

	stmt, err := db.Prepare("UPDATE user_model set Password = ?, Email = ?, Eggid = ?, Eggnames = ? where Username = ?")
	if err != nil {
		log.Fatal(err)
	}
	defer stmt.Close()

	res, err := stmt.Exec(ourPerson.Password, ourPerson.Email, ourPerson.Eggid, ourPerson.Eggnames, ourPerson.Username)
	if err != nil {
		log.Fatal(err)
	}

	affected, err := res.RowsAffected()
	if err != nil {
		log.Fatal(err)
	}

	return affected
}

/*
Deletes record of person from User database
inputs: databse reference and username in string
returns rows affected
*/
func DeletePerson(db *sql.DB, usernameToDelete string) int64 {

	stmt, err := db.Prepare("DELETE FROM user_model where Username = ?")
	if err != nil {
		log.Fatal(err)
	}
	defer stmt.Close()

	res, err := stmt.Exec(usernameToDelete)
	if err != nil {
		log.Fatal(err)
	}

	affected, err := res.RowsAffected()
	if err != nil {
		log.Fatal(err)
	}

	return affected
}
