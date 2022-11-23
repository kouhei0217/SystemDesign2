FetchMenus();
checkCookie();

function checkCookie() {
  if (document.cookie != "") {
    elements = document.getElementsByName("menus");
    console.log(elements);
    for (let i = 0; i < elements.length; i++) {
      elements.item(i).disabled = true;
      console.log(elements.item(i));
    }
    document.getElementById("vote_menu").disabled = true;
    document.getElementById(document.cookie[1]).checked = true;
  }
}
