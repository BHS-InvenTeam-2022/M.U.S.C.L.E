let fileContent;
const url = 'http://127.0.0.1:5000/egg';

document.getElementById('datainputbtn').addEventListener('change', function (e) {
    let file = document.getElementById('datainputbtn').files[0];
    (async () => {
        fileContent = await file.text();
        //console.log(fileContent);

    })();
});

function uploadData() {
    parseforapi(fileContent);
}

function parseforapi(datastring) {
    if (datastring == "") {
        alert("cannot send empyty message");
    }

    let temp = "";
    let ph = "";
    let salinity = "";
    let clock = "";
    let lines = datastring.split("\n");
    let username = "admin1"

    for (let i = 0; i < lines.length; i++) {
        let nums = lines[i].split(";");
        temp += nums[0] + ";";
        ph += nums[1] + ";";
        salinity += nums[2] + ";";

        clock += nums[3].slice(0, 20) + ";";

    }


    const response = new XMLHttpRequest();

    const data = JSON.stringify({
        'eggId': username,
        'temp_id': "1",
        'temp_data': temp,
        'ph_id': "1",
        'ph_data': ph,
        'clock_id': "1",
        'clock_data': clock,
        'salinity_id': "1",
        'salinity_data': salinity,
        "data_packet": [],
        "clock_packet": []
    }
    );

    response.open("POST", url)
    response.setRequestHeader('Content-Type', 'application/json');

    response.send(data);

    response.onload = (e) => {

        console.log(response);
    }
}

function getchoice() {
    var ele = document.getElementsByName('datatype');
    let eggId = "admin1";//change to obtain from session cookies
    let datatype;
    for (i = 0; i < ele.length; i++) {
        if (ele[i].checked)
            datatype = ele[i].value;
    }


    var datastring = `${url}?eggId=${eggId}&datatype=${datatype}`

    const response = new XMLHttpRequest();
    response.open("GET", datastring)


    response.send();
    response.onload = (e) => {
        console.log(response);
    }
}
