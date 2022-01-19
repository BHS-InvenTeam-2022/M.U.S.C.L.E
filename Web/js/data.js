let fileContent;
const url = 'http://127.0.0.1:5000/egg';
var myChart;

document.getElementById('datainputbtn').addEventListener('change', function (e) {
    let file = document.getElementById('datainputbtn').files[0];
    (async () => {
        fileContent = await file.text();
        console.log("done");
    })();
});

function uploadData() {
    parseforapi(fileContent);
}

function parseforapi(datastring) {
    if (datastring == "") {
        alert("cannot send empyty message");
    }
    if(sessionStorage){
        if(!sessionStorage.getItem("username")){
            alert("not logged in yet...");
            return;
        }
    }

    let temp = "";
    let ph = "";
    let salinity = "";
    let clock = "";
    let lines = datastring.split("\n");
    let username = sessionStorage.getItem("eggId");

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
    if(sessionStorage){
        if(!sessionStorage.getItem("username")){
            alert("not logged in yet...");
            return;
        }
    }
    let ele = document.getElementsByName('datatype');
    let eggId = sessionStorage.getItem("eggId");
    let datatype;
    for (i = 0; i < ele.length; i++) {
        if (ele[i].checked)
            datatype = ele[i].value;
    }
    let startdate = document.getElementById("startdate").value;
    let enddate = document.getElementById("enddate").value;

    if (!startdate || !enddate) {
        alert("dates not input correctly");
        return;
    } else {
        startdate = new Date(startdate);
        enddate = new Date(enddate);
    }

    var datastring = `${url}?eggId=${eggId}&datatype=${datatype}`

    const response = new XMLHttpRequest();
    response.open("GET", datastring)

    let data;
    response.send();
    response.onload = (e) => {
        data = JSON.parse(response.response);
        if (data) {
            let starti = null;
            let endi = null;
            let clock_x = data.clock_packet.slice(1, -1).split(",");
            clock_x = clock_x.map(function (ele) {
                ele = ele.slice(1, -1)
                return ele.replace("'", '');
            });
            console.log(clock_x);
            let data_y = data.data_packet.slice(1, -1).split(",");
            data_y = data_y.map(function (ele) {
                return parseFloat(ele);
            });

            document.getElementById('date-range').textContent = `date range is from ${clock_x[0]} to ${clock_x[clock_x.length - 1]}`;

            if (startdate.valueOf() < new Date(clock_x[0]).valueOf() || startdate.valueOf() > new Date(clock_x[clock_x.length - 1]).valueOf()) {
                alert("start date not in range");
                return;
            }
            if (enddate.valueOf() < new Date(clock_x[0]).valueOf() || enddate.valueOf() > new Date(clock_x[clock_x.length - 1]).valueOf()) {
                alert("end date not in range");
                return;
            }

            startstop = false;
            endstop = false;
            for (let i = 0; i < clock_x.length; i++) {
                const element = new Date(clock_x[i]);
                if (element.valueOf() >= startdate.valueOf() && !startstop) {
                    starti = i;
                    startstop = true;
                } else if (element.valueOf() >= enddate.valueOf() && !endstop) {
                    endi = i;
                    endstop = false;
                }
            }
            if (!starti || !endi) {
                alert("date not in range");
                return;
            }
            if (starti >= endi) {
                alert("startdate is equal to or greater than end date and that's just not possible");
                return;
            }
            clock_x = clock_x.slice(starti, endi);
            data_y = data_y.slice(starti, endi);
            chartIt(clock_x, data_y);

        } else {
            alert("no data was recieved");
            return;
        }

    }

}


function chartIt(xs, ys) {
    const ctx = document.getElementById('chart').getContext('2d');
    if (myChart) {
        myChart.destroy();
    }
    myChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: xs,
            datasets: [{
                label: 'Global Average Temperature',
                data: ys,
                backgroundColor: 'rgba(255, 99, 132, 0.2)',
                borderColor: 'rgba(255, 99, 132, 1)',
                borderWidth: 1
            }]
        },
        options: {
            scales: {
                y: {
                    beginAtZero: false
                }
            }
        }
    });


}

function readFromUpload() {
    if (!fileContent) {
        alert("no file uploaded");
        return;
    }

    let temp = "";
    let ph = "";
    let salinity = "";
    let clock = "";
    let lines = fileContent.split("\n");
    let data_y = [];
    var ele = document.getElementsByName('datatype');
    let datatype;
    let starti = 0;
    let endi = 0;
    for (i = 0; i < ele.length; i++) {
        if (ele[i].checked)
            datatype = ele[i].value;
    }

    for (let i = 0; i < lines.length; i++) {
        let nums = lines[i].split(";");
        temp += nums[0] + ";";
        ph += nums[1] + ";";
        salinity += nums[2] + ";";

        clock += nums[3].slice(0, 20) + ";";
    }

    clock = clock.split(";");
    clock = clock.map((ele) => {
        return ele.slice(0, 19);
    });

    switch (datatype) {
        case "temp":
            data_y = temp.split(";");
            break;
        case "ph":
            data_y = ph.split(";");
            break;
        case "salinity":
            data_y = salinity.split(";");
            break;
        default:
            break;
    }

    clock.pop();
    data_y.pop();

    let startdate = document.getElementById("startdate").value;
    let enddate = document.getElementById("enddate").value;

    if (!startdate || !enddate) {
        alert("dates not input correctly");
        return;
    } else {
        startdate = new Date(startdate);
        enddate = new Date(enddate);
    }



    document.getElementById('date-range').textContent = `date range is from ${clock[0]} to ${clock[clock.length - 1]}`;
    if (startdate.valueOf() < new Date(clock[0]).valueOf() || startdate.valueOf() > new Date(clock[clock.length - 1]).valueOf()) {
        alert("start date not in range");
        return;
    }
    if (enddate.valueOf() < new Date(clock[0]).valueOf() || enddate.valueOf() > new Date(clock[clock.length - 1]).valueOf()) {
        alert("end date not in range");
        return;
    }

    startstop = false;
    endstop = false;
    for (let i = 0; i < clock.length; i++) {
        const element = new Date(clock[i]);
        if (element.valueOf() >= startdate.valueOf() && !startstop) {
            starti = i;
            startstop = true;
        } else if (element.valueOf() >= enddate.valueOf() && !endstop) {
            endi = i;
            endstop = false;
        }
    }
    if (!starti || !endi) {
        alert("date not in range");
        return;
    }
    if (starti >= endi) {
        alert("startdate is equal to or greater than end date and that's just not possible");
        return;
    }
    clock = clock.slice(starti, endi);
    data_y = data_y.slice(starti, endi);

    chartIt(clock, data_y);
}