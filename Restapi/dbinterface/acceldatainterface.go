package dbinterface

import (
	"database/sql"
	"fmt"
	"log"
	"strings"
)

type AccelSensor struct {
	Id    int     `json:"Id"`
	Eggid string  `json:"EggId"`
	Angle float64 `json:"angle"`
}

/*
checks if a slice of string is acceptable to input into the database and returns bool
*/
func checkIfValidEntryGape(entry []string) bool {
	if len(entry) != 3 {
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
func AddAccelRecords(db *sql.DB, datastr string, eggid string) {
	var str strings.Builder
	str.WriteString("INSERT INTO sonde_model (eggId, accel_data, clock_data) VALUES ")
	lines := splitstring(datastr)

	linesadded := 0
	for r := range lines {
		split := strings.Split(r, ";")
		if checkIfValidEntryGape(split) {
			str.WriteString(fmt.Sprintf("('%s', '%s', '%s'),", eggid, split[0], split[1]))
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
Reads from accelerometer database and returns two strings first being the actual data and second is corresponding clock data both align to each other in one-to-one relationship
*/
func ReadAccelRecords(db *sql.DB, eggid string) (string, string) {
	var datastr strings.Builder
	var clockstr strings.Builder

	var rows *sql.Rows
	var err error

	rows, err = db.Query("SELECT accel_data, clock_data FROM sonde_model WHERE eggId = '" + eggid + "'")

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
