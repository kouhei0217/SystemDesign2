import os

from flask import Flask, render_template, request, url_for

app = Flask(__name__)
app.config["MAX_CONTENT_LENGTH"] = 100 * 1024 * 1024


@app.context_processor
def override_url_for():
    return dict(url_for=dated_url_for)


def dated_url_for(endpoint, **values):
    if endpoint == 'static':
        filename = values.get('filename', None)
        if filename:
            file_path = os.path.join(app.root_path,
                                     endpoint, filename)
            values['q'] = int(os.stat(file_path).st_mtime)
    return url_for(endpoint, **values)


@app.route("/", methods=["GET", "POST"])
def Web():
    if request.method == "GET":
        return render_template("index.html")
    elif request.method == "POST":
        fs = request.files["image"]
        imagePath = "./static/menu.jpg"
        fs.save(imagePath)
        return "ok"


if __name__ == "__main__":
    app.run(host="127.0.0.1", port=8080, debug=True)
