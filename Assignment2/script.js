var pages = document.getElementsByClassName("page");
for (let page of pages) {
	if (page.id != "login"){
		page.style.display = "none";
	}
}

var links = document.getElementsByClassName("direct");
for (let link of links) {
	link.onclick = function() {
		for(let page of pages) {
			console.log(page);
			if(page.id == link.name) {
				page.style.display = "inline";
			} else {
				page.style.display = "none";
			}
		}
	}
}