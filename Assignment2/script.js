// User model
function user(username, password, phone, gid) {
	this.username = username;
	this.password = password;
	this.phone = phone;
	this.gid = gid;
	this.status = false;
}

// FOR STATUS: TRUE IS OPEN, FALSE IS CLOSED

// Init database, add two default users
var users = [];
users.push(new user("Ethan", "pass1", "123", "1010"));
var p = new user("Jack", "pass2", "321", "9999");
p.status = true;
users.push(p);

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

// Defualt action of all link buttons is to direct to their name's corresponding page
var links = document.getElementsByClassName("direct");
for (let link of links) {
	link.onclick = function() {
		showPage(link.name);
	}
}

document.getElementById("signup").onclick = function() {
	var username = document.getElementById("newuser").value;
	var password = document.getElementById("newpass").value;
	var phone = document.getElementById("newphone").value;
	var gid = document.getElementById("newID").value;
	var newUser = new user(username, password, phone, gid);
	users.push(newUser);
	currentUser = newUser;
	showPage("dashboard");
}

document.getElementById("sendemail").onclick = function () {
	// WOW, super clean way to generate random characters, thanks https://stackoverflow.com/questions/1349404/generate-random-string-characters-in-javascript
	alert("Your new password is " + Math.random().toString(36).substring(4));
}

document.getElementById("dologin").onclick = function () {
	var username = document.getElementById("currentuser").value;
	var password = document.getElementById("currentpass").value;
	for (let user of users) {
		if (user.username = username && user.password == password){
			currentUser = user;

			var isOpen = currentUser.status;
			for (let p of document.getElementsByClassName("status")) {
				p.style.color = isOpen ? "Green" : "Red";
				p.innerHTML = isOpen ? "OPEN" : "CLOSED";
			}
			document.getElementById("open").disabled = isOpen ? true : false;
			document.getElementById("close").disabled = isOpen ? false : true;

			showPage("dashboard");
			return;
		}
	}
	alert("Either your username or password is incorrect");
}

document.getElementById("open").onclick = function () {
	if (!currentUser.status) {
		for (let p of document.getElementsByClassName("status")) {
			p.style.color = "Orange";
			p.innerHTML = "Opening...";
			document.getElementById("open").disabled = true;
			document.getElementById("close").disabled = false;
			setTimeout(function () {
				p.style.color = "Green";
				p.innerHTML = "Open";
			}, 5000);
		}

		currentUser.status = !currentUser.status;
	}

}


document.getElementById("close").onclick = function () {
	if (currentUser.status) {
		for (let p of document.getElementsByClassName("status")) {
			p.style.color = "Orange";
			p.innerHTML = "Closing...";
			document.getElementById("open").disabled = false;
			document.getElementById("close").disabled = true;
			setTimeout(function () {
				p.style.color = "Red";
				p.innerHTML = "Closed";
			}, 5000);
		}
		currentUser.status = !currentUser.status;
	}
}

// Default view
showPage("login");









