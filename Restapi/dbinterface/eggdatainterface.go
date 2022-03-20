package dbinterface

import (
	"database/sql"
	"fmt"
	"log"
	"strings"

	_ "github.com/mattn/go-sqlite3"
)

type Egg struct {
	Id            int     `json:"Id"`
	Eggid         string  `json:"EggId"`
	Temp_data     float64 `json:"TempData"`
	Ph_data       float64 `json:"PhData"`
	Clock_data    string  `json:"ClockData"`
	Salinity_data float64 `json:"SalinityData"`
}

func PrepareEggDB(db *sql.DB) {
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

	stmt, _ = db.Prepare(`CREATE TABLE IF NOT EXISTS sonde_model (
		id INTEGER NOT NULL,
		"eggId" VARCHAR(75) NOT NULL,
		accel_data FLOAT NOT NULL,
		clock_data VARCHAR(25) NOT NULL,
		PRIMARY KEY (id)
	)`)
	defer stmt.Close()
	stmt.Exec()
}

type dataChan chan string

func (f dataChan) Next() *string {
	c, ok := <-f
	if !ok {
		return nil
	}
	return &c
}

/*
Generator function similiar to python yield functions using channels to add pieces of data to not load full string into memory
returns the data channel where data can then be called from using the Next() function
*/
func splitstring(data string) dataChan {
	/*f := splitstring(`75.4;4.3;0.3;2022-01-16 13:40:00
	78.5;7.4;1.0;2022-01-16 13:50:00
	77.7;6.8;0.3;2022-01-16 14:00:00
	77.0;5.5;1.0;2022-01-16 14:10:00`)
		fmt.Printf("%v\n", *f.Next())
		split := strings.Split(*f.Next(), ";")
		fmt.Println(split)
		for r := range f {
			fmt.Printf("%v ", r)
		}*/
	c := make(chan string)

	go func() {
		mystr := ""
		for _, letter := range data {

			if string(letter) == "\n" {
				c <- mystr
				mystr = ""
			} else {
				mystr += string(letter)
			}
		}
		close(c)
	}()
	return c
}

/*
checks if a slice of string is acceptable to input into the database and returns bool
*/
func checkIfValidEntry(entry []string) bool {
	if len(entry) != 5 {
		return false
	}
	for _, v := range entry {
		if v == "" {
			return false
		}
	}
	return true
}

/*
Adds records to database provided with the eggid provided and the data in a string in csv format with columns aligning to database columns
*/
func AddEggRecords(db *sql.DB, datastr string, eggid string) {
	var str strings.Builder
	str.WriteString("INSERT INTO egg_model (eggId, temp_data, ph_data, clock_data, salinity_data) VALUES ")
	lines := splitstring(datastr)

	linesadded := 0
	for r := range lines {
		split := strings.Split(r, ";")
		if checkIfValidEntry(split) {
			str.WriteString(fmt.Sprintf("('%s', '%s', '%s', '%s', '%s'),", eggid, split[0], split[1], split[3], split[2]))
			linesadded++
		}
	}

	if linesadded == 0 {
		log.Fatal("no entries added to database")
	}

	mystr := str.String()
	mystr = mystr[:len(mystr)-1]

	tx, err := db.Begin()

	if err != nil {
		log.Fatal(err)
	}

	res, err := tx.Exec(mystr)
	_ = res
	if err != nil {
		fmt.Println("error in committing")
		log.Fatal(err)
	}
	tx.Commit()
}

/*
Reads from egg database and returns two strings first being the actual data and second is corresponding clock data both align to each other in one-to-one relationship
*/
func ReadEggRecords(db *sql.DB, datatype string, eggid string) (string, string) {
	var datastr strings.Builder
	var clockstr strings.Builder

	var rows *sql.Rows
	var err error

	if datatype == "ph" {
		rows, err = db.Query("SELECT ph_data, clock_data FROM egg_model WHERE eggId = '" + eggid + "'")
	} else if datatype == "temp" {
		rows, err = db.Query("SELECT temp_data, clock_data FROM egg_model WHERE eggId = '" + eggid + "'")
	} else if datatype == "salinity" {
		rows, err = db.Query("SELECT salinity_data, clock_data FROM egg_model WHERE eggId = '" + eggid + "'")
	} else {
		return "", ""
	}

	if err != nil {
		log.Fatal(err)
	}
	err = rows.Err()
	if err != nil {
		log.Fatal(err)
	}
	defer rows.Close()

	for rows.Next() {
		var temp_data string
		var temp_clock string
		err = rows.Scan(&temp_data, &temp_clock)
		if err != nil {
			log.Fatal(err)
		}
		datastr.WriteString(temp_data)
		datastr.WriteString(";")
		clockstr.WriteString(temp_clock)
		clockstr.WriteString(";")
	}

	return datastr.String(), clockstr.String()
}
