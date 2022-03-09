package dbinterface

type GapeSensor struct {
	Id     int     `json:"Id"`
	Eggid  string  `json:"EggId"`
	Accel1 float64 `json:"sensor-1"`
	Accel2 float64 `json:"sensor-2"`
}
