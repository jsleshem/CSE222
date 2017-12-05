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

var particleLogin = 'ethandjay@gmail.com';
var particlePassword = 'Kronos97!';
var deviceId = '27001f001951353337343731';  // Comes from the number in the particle.io Console

function sendAutoCloseState() {
	if (currentUser.autoClose) {
		particle.callFunction({ deviceId: deviceId, name: 'autoCloseSet', argument: String(currentUser.autoCloseTime), auth: token }).then(callSuccess, callFailure);
	} else {
		particle.callFunction({ deviceId: deviceId, name: 'autoCloseSet', argument: "FALSE", auth: token }).then(callSuccess, callFailure);
	}
}

// Call back function for login success/failure
function loginSuccess(data) {
    console.log('API call completed on promise resolve: ', data.body.access_token);
    token = data.body.access_token;

    particle.getEventStream({ deviceId: 'mine', auth: token }).then(function(stream) {
	  stream.on('state-change', function(data) {
	  	console.log(data);
	  	currentUser.status = data.data;
	  	setStatusofUI();
	  });
	});

	sendAutoCloseState();
	getStatus();
}

function loginError(error) {
    console.log('API call completed on promise fail: ', error);
}

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

// Set UI to represent state of garage

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




// Helpers for signup

var productId = "6238";

var clientId = "garage-door-app-2719";
var clientToken = "3d6660ac010f48513147d74f72e090240829c79e";

var deviceOneId = "27001f001951353337343731";
var deviceTwoId = "2f0049000c47343438323536";
var customerToken = "LEAVE ALONE";

function saveTokenAndClaimDeviceOne(data) {
 console.log("Success creating customer; Claiming Device One");
 console.dir(data)
 customerToken = data.body.access_token;
 // Return a "promise" object (so .then() can be used)
 return particle.claimDevice({deviceId:deviceOneId, requestTransfer:true, auth:customerToken})
}

function claimDeviceTwo(data) {
 console.log("Success claiming device one; Claiming Device Two");
 console.dir(data)
 // Return a "promise" object (so .then() can be used)
 return particle.claimDevice({deviceId:deviceTwoId, requestTransfer:true, auth:customerToken})
}

function doneClaimingDevices(newUser) {
 	console.log("Done Claiming Devices");
	users.push(newUser);
	currentUser = newUser;
	particle.login({username: newUser.username, password: newUser.password}).then(loginSuccess, loginError);
	showPage("dashboard");
}

function errorClaimingDevices() {
 console.log("Error Claiming Devices");
}

// Sign up button event, creates account and claims devices
document.getElementById("signup").addEventListener("click", function() {
		var email = document.getElementById("newuser").value;
		var password = document.getElementById("newpass").value;
		var newUser = new user(email, password, 0, "name", 0);


		particle.createCustomer = function ({ productId, clientId, clientToken, customerEmail, customerPassword }) {
			 const auth = clientId + ':' + clientToken;
			 const uri = `/v1/products/${productId}/customers`;
			 return this.post(uri, {productIdOrSlug:productId, client_id:clientId, client_secret:clientToken, email:customerEmail, password:customerPassword
			 }, auth, this.context);
		}

		particle.createCustomer( {productId:productId, clientId:clientId, clientToken:clientToken, customerEmail:email, customerPassword:password} )
		 .then(saveTokenAndClaimDeviceOne)
		 .then(claimDeviceTwo)
		 .then(function() { doneClaimingDevices(newUser); })
		 .catch(errorClaimingDevices)

	}
)

// Password reset event
document.getElementById("sendemail").addEventListener("click", function () {
		// WOW, super clean way to generate random characters, thanks https://stackoverflow.com/questions/1349404/generate-random-string-characters-in-javascript
		alert("Your new password is " + Math.random().toString(36).substring(4));
	}
)

// Renames garage
document.getElementById("rename").addEventListener("click", function () {
		currentUser.name = document.getElementById("newGarageName").value;
		document.getElementById("newGarageName").value = "";
		document.getElementById("garageName").innerHTML = currentUser.name;
	}
)

// AutoClose checkbox action
document.getElementById("auto-close-check").addEventListener("click", function () {
		currentUser.autoClose = !currentUser.autoClose;

		if (currentUser.autoClose && document.getElementById("close-sec").value) {
			currentUser.autoCloseTime = document.getElementById("close-sec").value * 1000;
		} else if (currentUser.autoClose) {
			document.getElementById("close-sec").value = 5;
			currentUser.autoCloseTime = 5000;
		}

		sendAutoCloseState();
	}
)

// Login button action
document.getElementById("dologin").addEventListener("click", function () {
		var username = document.getElementById("currentuser").value;
		var password = document.getElementById("currentpass").value;
		for (let user of users) {
			if (user.username = username && user.password == password){
				currentUser = user;

				console.log(login + " " + password);
				particle.login({username: particleLogin, password:particlePassword}).then(loginSuccess, loginError);

				document.getElementById("garageName").innerHTML = currentUser.name;
				document.getElementById("auto-close-check").checked = currentUser.autoClose;

				showPage("dashboard");
				return;
			}
		}
		alert("Either your username or password is incorrect");
	}
)


	for (let p of document.getElementsByClassName("close")){
		p.addEventListener("keyup", function () {
			if (p.value && (p.value > 59 || p.value < 0)){
				document.getElementById("auto-close-check").disabled = true;
			} else {
				document.getElementById("auto-close-check").disabled = false;
			}
		}
		)
	}

// Garage button action
document.getElementById("button").addEventListener("click", function() {
	particle.callFunction({ deviceId: deviceId, name: 'buttonPress', argument: "", auth: token }).then(callSuccess, callFailure);
});


	
// );

// Default view
showPage("login");









