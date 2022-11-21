let APIaddress = "34.105.1.151";

// 先月のNo.1メニューの名前を取得
function FetchMenu() {
  fetch("http://" + APIaddress + "/fetch-menu")
    .then(function (response) {
      return response.json();
    })
    .then(function (data) {
      popular_menu.innerHTML = data.menu_name;
    });
}

// 過去のメニューの名前を取得
function FetchMenus() {
  fetch("http://" + APIaddress + "/fetch-menus")
    .then(function (response) {
      return response.json();
    })
    .then(function (data) {
      let menusHTML = "";
      for (let i = 0; i < data.menus.length; i++) {
        menusHTML +=
          "<input type='radio' name='menus' value=" +
          data.menus[i].menu_id +
          ">" +
          data.menus[i].menu_name;
      }
      menusHTML.innerHTML = menusHTML;
    });
}

// リクエストしたいメニューを投票
function VoteMenu() {
  elements = document.getElementsByName("names");
  for (let i = 0; i < elements.length; i++) {
    if (elements.item(i).checked) {
      fetch("http://" + APIaddress + "/vote-menu", {
        headers: { "Content-Type": "application/json" },
        method: "POST",
        body: JSON.stringify({ menu_id: elements.item(i).value }),
      });
      break;
    }
  }
  fetch("http://" + APIaddress + "/vote-menu", {
    headers: { "Content-Type": "application/json" },
    method: "POST",
    body: JSON.stringify({ menu_id: 1 }),
  });
}

// 投票用のメニューを追加
function AddMenu() {
  fetch("http://" + APIaddress + "/add-menu", {
    headers: { "Content-Type": "application/json" },
    method: "POST",
    body: JSON.stringify({
      menu_name: document.getElementsById("menu_adder").value,
    }),
  });
}
