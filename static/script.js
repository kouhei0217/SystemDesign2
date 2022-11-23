FetchMenus();

function CheckCookie() {
  if (document.cookie != "") {
    elements = document.getElementsByName("menus");
    console.log(elements);
    for (let i = 0; i < elements.length; i++) {
      elements.item(i).disabled = true;
      console.log(elements.item(i));
    }
    document.getElementById("vote_menu").disabled = true;
    document.getElementById(document.cookie.split("=")[1]).checked = true;
  }
}
