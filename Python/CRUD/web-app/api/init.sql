CREATE TABLE reviews (
    id SERIAL PRIMARY KEY,
    movie_id INTEGER,
    rating NUMERIC(3,1),
    review TEXT,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    user_id INTEGER,
    CONSTRAINT reviews_rating_check CHECK (rating >= 0 AND rating <= 10),
    CONSTRAINT fk_movie FOREIGN KEY (movie_id) REFERENCES movies(id),
    CONSTRAINT fk_user FOREIGN KEY (user_id) REFERENCES users(user_id)
);

CREATE TABLE users (
    user_id SERIAL PRIMARY KEY,
    username TEXT NOT NULL UNIQUE,
    email TEXT NOT NULL UNIQUE,
    password_hash BYTEA NOT NULL
);

CREATE TABLE movies (
    id SERIAL PRIMARY KEY,
    title TEXT NOT NULL,
    year INTEGER,
    director TEXT
);

INSERT INTO movies (title, year, director) VALUES
('Toy Story', 1995, 'John Lasseter'),
('A Bug''s Life', 1998, 'John Lasseter'),
('Toy Story 2', 1999, 'John Lasseter'),
('Monsters, Inc.', 2001, 'Pete Docter'),
('Finding Nemo', 2003, 'Andrew Stanton'),
('The Incredibles', 2004, 'Brad Bird'),
('Cars', 2006, 'John Lasseter'),
('Ratatouille', 2007, 'Brad Bird'),
('WALL·E', 2008, 'Andrew Stanton'),
('Up', 2009, 'Pete Docter'),
('Toy Story 3', 2010, 'Lee Unkrich'),
('Cars 2', 2011, 'John Lasseter'),
('Brave', 2012, 'Mark Andrews and Brenda Chapman'),
('Monsters University', 2013, 'Dan Scanlon'),
('Inside Out', 2015, 'Pete Docter'),
('The Good Dinosaur', 2015, 'Peter Sohn'),
('Finding Dory', 2016, 'Andrew Stanton'),
('Cars 3', 2017, 'Brian Fee'),
('Coco', 2017, 'Lee Unkrich and Adrian Molina'),
('Incredibles 2', 2018, 'Brad Bird'),
('Toy Story 4', 2019, 'Josh Cooley'),
('Onward', 2020, 'Dan Scanlon'),
('Soul', 2020, 'Pete Docter'),
('Luca', 2021, 'Enrico Casarosa'),
('Turning Red', 2022, 'Domee Shi'),
('Lightyear', 2022, 'Angus MacLane'),
('Elemental', 2023, 'Peter Sohn'),
('Inside Out 2', 2024, 'Kelsey Mann');