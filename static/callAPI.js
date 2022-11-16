// 先月のNo.1メニューの名前を取得
function FetchMenu() {
  fetch("http://34.145.89.163/fetch-menu")
    .then(function (response) {
      return response.json();
    })
    .then(function (data) {
      popular_menu.innerHTML = data.menu_name;
    });
}

// 過去のメニューの名前を取得
function FetchMenus() {
  fetch("http://34.145.89.163/fetch-menus")
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
  for (let i = 0; i < len; i++) {
    if (elements.item(i).checked) {
      fetch("http://34.145.94.229/vote-menu", {
        headers: {
          Accept: "application/json",
          "Content-Type": "application/json",
        },
        method: "POST",
        body: JSON.stringify({ menu_id: elements.item(i).value }),
      });
      break;
    }
  }
}

// 投票用のメニューを追加
function AddMenu() {
  fetch("http://34.145.94.229/add-menu", {
    headers: {
      Accept: "application/json",
      "Content-Type": "application/json",
    },
    method: "POST",
    body: JSON.stringify({
      menu_name: document.getElementsById("menu_adder").value,
    }),
  });
}
