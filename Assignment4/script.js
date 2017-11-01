// User model
function user(username, password, phone, name, gid) {
	this.username = username;
	this.password = password;
	this.phone = phone;
	this.name = name;
	this.gid = gid;
	this.status = 0;
	this.autoClose = false;
	this.autoCloseTime = 0;
}

// FOR STATUS: TRUE IS OPEN, FALSE IS CLOSED

// Init database, add two default users
var users = [];
users.push(new user("Ethan", "pass1", "123", "Garage1", "1010"));
var p = new user("Jack", "pass2", "321", "Garage2", "9999");
users.push(p);

// STATUS EQUIVALENTS:
// --------------------
// OPENED: 0
// CLOSED: 1
// OPENING: 2
// CLOSING: 3
// PAUSED: 4
// FAULTED: 5

// Current user
var currentUser = null;

// Only show login at first
var pages = document.getElementsByClassName("page");

// Page switch helper
function showPage(id) {
	if (id == "login") {
		currentUser = null;
	}
	for(let page of pages) {
		if(page.id == id) {
			page.style.display = "inline";
		} else {
			page.style.display = "none";
		}
	}
}

var particle = new Particle();
var token;
var openedTimer;

var login = 'ethandjay@gmail.com';
var password = 'Kronos97!';
var deviceId = '27001f001951353337343731';  // Comes from the number in the particle.io Console

// Call back function for login success/failure
function loginSuccess(data) {
    console.log('API call completed on promise resolve: ', data.body.access_token);
    token = data.body.access_token;

	particle.getEventStream({ deviceId: 'mine', auth: token }).then(function(stream) {
	  stream.on('state-change', function(data) {
	  	console.log(data);
	  	currentUser.status = data.data;
	  	setStatusofUI();
	  	if (openedTimer) {
	  		clearTimeout(openedTimer);
	  	}
		if(currentUser.autoClose && currentUser.status == 0) {
			openedTimer = setTimeout( function() {
				particle.callFunction({ deviceId: deviceId, name: 'buttonPress', argument: "", auth: token }).then(callSuccess, callFailure);
			}, currentUser.autoCloseTime);
		}
	  });
	});
}

function loginError(error) {
    console.log('API call completed on promise fail: ', error);
}

// Try to login
particle.login({username: login, password:password}).then(loginSuccess, loginError);

function callSuccess(data) {
    console.log('Function called succesfully:', data);
}

function callFailure(error) {
    console.log('An error occurred:', error);
}

function getStatusSuccess(data) {
	currentUser.status = data.body.return_value;
	setStatusofUI();
}

function getStatus() {
	particle.callFunction({ deviceId: deviceId, name: 'whichState', argument: '', auth: token }).then(getStatusSuccess, callFailure);
	console.log("hi");
}


function setStatusofUI() {
	var status = currentUser.status;
	status = parseInt(currentUser.status);
	if (openedTimer){
		clearInterval(openedTimer);
	}

	for (let p of document.getElementsByClassName("control")){
		p.disabled = false;
	}

	console.log("Status is " + status);
	for (let p of document.getElementsByClassName("status")) {
		
		switch(status){
		case 0:
			p.style.color = "Green";
			p.innerHTML = "OPENED";
			break;
		case 1:
			p.style.color = "Green";
			p.innerHTML = "CLOSED";
			break;
		case 2:
			p.style.color = "Orange";
			p.innerHTML = "OPENING";
			break;
		case 3:
			p.style.color = "Orange";
			p.innerHTML = "CLOSING";
			break;
		case 4:
			p.style.color = "Yellow";
			p.innerHTML = "PAUSED";
			break;
		case 5:
			p.style.color = "RED";
			p.innerHTML = "FAULTED";
			break;
		}
	}
}


// Defualt action of all link buttons is to direct to their name's corresponding page
var links = document.getElementsByClassName("direct");
for (let link of links) {
		link.addEventListener("click", function() {
			showPage(link.name);
		}
	)
}

document.getElementById("signup").addEventListener("click", function() {
		var username = document.getElementById("newuser").value;
		var password = document.getElementById("newpass").value;
		var phone = document.getElementById("newphone").value;
		var name = document.getElementById("newname").value;
		var gid = document.getElementById("newID").value;
		var newUser = new user(username, password, phone, name, gid);
		users.push(newUser);
		currentUser = newUser;
		showPage("dashboard");
	}
)

document.getElementById("sendemail").addEventListener("click", function () {
		// WOW, super clean way to generate random characters, thanks https://stackoverflow.com/questions/1349404/generate-random-string-characters-in-javascript
		alert("Your new password is " + Math.random().toString(36).substring(4));
	}
)

document.getElementById("rename").addEventListener("click", function () {
		currentUser.name = document.getElementById("newGarageName").value;
		document.getElementById("newGarageName").value = "";
		document.getElementById("garageName").innerHTML = currentUser.name;
	}
)

document.getElementById("auto-close-check").addEventListener("click", function () {
		currentUser.autoClose = !currentUser.autoClose;

		if (currentUser.autoClose && document.getElementById("close-sec").value) {
			currentUser.autoCloseTime = document.getElementById("close-sec").value * 1000;
		} else if (currentUser.autoClose) {
			document.getElementById("close-sec").value = 5;
			currentUser.autoCloseTime = 5000;
		}
		
		if(currentUser.autoClose && currentUser.status == 0) {
			setTimeout( function() {
				particle.callFunction({ deviceId: deviceId, name: 'buttonPress', argument: "", auth: token }).then(callSuccess, callFailure);
			}, currentUser.autoCloseTime);
		}
	}
)

document.getElementById("dologin").addEventListener("click", function () {
		var username = document.getElementById("currentuser").value;
		var password = document.getElementById("currentpass").value;
		for (let user of users) {
			if (user.username = username && user.password == password){
				currentUser = user;

				getStatus();

				document.getElementById("garageName").innerHTML = currentUser.name;
				document.getElementById("auto-close-check").checked = currentUser.autoClose;

				showPage("dashboard");
				return;
			}
		}
		alert("Either your username or password is incorrect");
	}
)

document.getElementById("button").addEventListener("click", function() {
	particle.callFunction({ deviceId: deviceId, name: 'buttonPress', argument: "", auth: token }).then(callSuccess, callFailure);
});

// document.getElementById("open").addEventListener("click", function () {

// 	particle.callFunction({ deviceId: deviceId, name: 'changeState', argument: "OPENING", auth: token }).then(callSuccess, callFailure);


// 		if (!currentUser.status) {
// 			for (let p of document.getElementsByClassName("status")) {
// 				p.style.color = "Orange";
// 				p.innerHTML = "Opening...";
// 				document.getElementById("open").disabled = true;
// 				document.getElementById("close").disabled = false;
// 				setTimeout(function () {
// 					p.style.color = "Green";
// 					p.innerHTML = "Open";
// 				}, 5000);
// 			}

// 			currentUser.status = !currentUser.status;
// 		}

// 	}
// )


// document.getElementById("close").addEventListener("click",function () {
// 		if (currentUser.status) {
// 			for (let p of document.getElementsByClassName("status")) {
// 				p.style.color = "Orange";
// 				p.innerHTML = "Closing...";
// 				document.getElementById("open").disabled = false;
// 				document.getElementById("close").disabled = true;
// 				setTimeout(function () {
// 					p.style.color = "Red";
// 					p.innerHTML = "Closed";
// 				}, 5000);
// 			}
// 			currentUser.status = !currentUser.status;
// 		}
// 	}
	


	
// );

// Default view
showPage("login");









