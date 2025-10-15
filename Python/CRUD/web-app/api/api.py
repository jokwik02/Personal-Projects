from db import *
import time
from flask import Flask, jsonify, request
from flask_cors import CORS
import jwt
import datetime
from functools import wraps

#Converts SQL tuples into dictionary form to easily jsonify it
def convert_to_dict(tabl):
    cols = [desc[0] for desc in cursor.description]
    return [dict(zip(cols, row)) for row in tabl]

#For user authentication
def token_required(f):
    @wraps(f)
    def decorated(*args, **kwargs):
        token = request.headers.get("Authorization")
        if not token:
            return jsonify({"error": "Missing token"}), 403

        try:
            data = jwt.decode(token.split()[1], app.config["SECRET_KEY"], algorithms=["HS256"])
            user = get_user(data["email"])
            if not user:
                return jsonify({"error": "Invalid token"}), 403
        except Exception as e:
            return jsonify({"error": f"Invalid token: {str(e)}"}), 403

        return f(user, *args, **kwargs)
    return decorated

app = Flask(__name__)
CORS(app,supports_credentials=True)
app.config["SECRET_KEY"] = os.getenv('SECRET_KEY')

#Sends info about all movies to frontend to be displayed
@app.route('/movies', methods=['GET'])
def get_movies():
    movies = get_db_movies()
    return jsonify(convert_to_dict(movies))

#Sends info about selected movies to frontend to be displayed
@app.route('/movies/<int:movie_id>', methods=['GET'])
def get_movie(movie_id):
    movie = get_movie_info(movie_id)
    return jsonify(convert_to_dict(movie))

#Sends all reviews for selected movie to be displayed
@app.route('/movies/reviews/<int:movie_id>', methods=['GET'])
def get_movie_reviews(movie_id):
	reviews = get_reviews_from_movie(movie_id, "r.updated_at DESC")
	return jsonify(convert_to_dict(reviews))

#Checks login credentials
@app.route("/login", methods=["POST"])
def login():
    data = request.json
    email = data.get("email")
    password = data.get("password")
    if check_user(email, password):
        token = jwt.encode(
            {"email": email, "exp": datetime.datetime.utcnow() + datetime.timedelta(hours=1)},
            app.config["SECRET_KEY"],
            algorithm="HS256"
        )
        return jsonify({"token": token, "email": email, "username": get_username(email)}), 200
    else:
         return jsonify({"error": "Wrong email or password"}), 401
    
#Registers a user
@app.route("/register", methods=["POST"])
def register():
    data = request.json
    email = data.get("email")
    password = data.get("password")
    username = data.get("username")
    if create_user(username, email, password):
        token = jwt.encode(
            {"email": email, "exp": datetime.datetime.utcnow() + datetime.timedelta(hours=1)},
            app.config["SECRET_KEY"],
            algorithm="HS256"
        )
        return jsonify({"token":token, "message": "Registration succesfull"}), 200
    else:
        return jsonify({"error": "Username or email already exists"}), 400
    
#Updates an already existing review
@app.route("/edit_review/<int:review_id>", methods=["PUT"])
@token_required
def edit_review(user, review_id):
    data = request.json
    review = data.get("review")
    rating = data.get("rating")
    update_review(rev_id=review_id, rating= rating, review=review)
    return jsonify({"message": "Edit succesfull"}), 200

#Adds a review for a movie
@app.route("/add_review", methods=["POST"])
@token_required
def add_review(user):
    data = request.json
    review = data.get("review")
    rating = data.get("rating")
    movie_id = data.get("movie_id")
    insert_review(movie_id, user[0], rating, review)
    return jsonify({"message": "Added review succesfully"}), 200

#Deletes a review
@app.route("/delete_review/<int:review_id>", methods=["DELETE"])
@token_required
def delete_review(user, review_id):
    remove_review(review_id)
    return jsonify({"message": "Deletion succesfull"}), 200


    