const url = 'http://127.0.0.1:8000/user';
function validate() {
    var username = document.getElementById("username").value;
    var password = document.getElementById("password").value;

    var datastring = `${url}?username=${username}&password=${password}`

    const response = new XMLHttpRequest();
    response.open("GET", datastring)


    response.send();
    response.onload = (e) => {
        console.log(response);
        if(response.status == 200){
            serverresponse = JSON.parse(response.response);
            console.log(serverresponse)
            sessionStorage.setItem("Username",serverresponse.Username);
            sessionStorage.setItem("EggId",serverresponse.EggId);
            sessionStorage.setItem("Eggnames",serverresponse.Eggnames);
            window.location.href = "./chart.html";
        }else{
            alert(JSON.parse(response.response).message);
            return;
        }
    }


}
