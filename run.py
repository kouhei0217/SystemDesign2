import base64
import os

from flask import Flask, Response, render_template, request, url_for
from flask_cors import CORS

from python.handleDatabase import fetchMenus, voteMenu

app = Flask(__name__)
app.config["MAX_CONTENT_LENGTH"] = 100 * 1024 * 1024
CORS(
    app,
    supports_credentials=True
)


@app.context_processor
def override_url_for():
    return dict(url_for=dated_url_for)


def dated_url_for(endpoint, **values):
    if endpoint == "static":
        filename = values.get("filename", None)
        if filename:
            file_path = os.path.join(app.root_path,
                                     endpoint, filename)
            values["q"] = int(os.stat(file_path).st_mtime)
    return url_for(endpoint, **values)


@app.route("/", methods=["GET"])
def Web():
    return render_template("index.html")


@app.route("/fetch-image", methods=["GET"])
def FetchImage():
    with open("static/menu.jpg", "rb") as file:
        imageBase64 = base64.b64encode(file.read()).decode("utf-8")
    return {"image": imageBase64}


@app.route("/save-image", methods=["POST"])
def SaveImage():
    fs = request.files["image"]
    imagePath = "./static/menu.jpg"
    fs.save(imagePath)
    return Response(status=204)


@app.route("/fetch-menu", methods=["GET"])
def FetchMenu():
    return {"name": "s1"}


@app.route("/fetch-menus", methods=["GET"])
def FetchMenus():
    print(type(fetchMenus()))
    return fetchMenus()


@app.route("/vote-menu", methods=["POST"])
def VoteMenu():
    voteMenu(request.data["id"])
    return Response(status=204)


if __name__ == "__main__":
    app.run(host="127.0.0.1", port=8080, debug=True)
