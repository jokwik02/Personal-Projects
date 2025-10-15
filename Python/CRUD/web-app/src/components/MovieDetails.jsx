import React, { useEffect, useState } from "react";
import { useParams } from "react-router-dom";
import { CircularProgress, Typography, Paper } from "@mui/material";
import MovieReviews from "./MovieReviews";
import API_BASE_URL from "../config";

const MovieDetails = () => {
  const { id } = useParams();
  const [movie, setMovie] = useState(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    fetch(`${API_BASE_URL}/movies/${id}`)
      .then((response) => response.json())
      .then((data) => {
		if (Array.isArray(data) && data.length > 0) {
		  setMovie(data[0]); // Extract the first movie object
		} else {
		  setMovie(null);
		}
		setLoading(false);
	  })
	  
      .catch((error) => {
        console.error("Error fetching movie details:", error);
        setLoading(false);
      });
  }, [id]);

  if (loading) {
    return <CircularProgress style={{ display: "block", margin: "20px auto" }} />;
  }

  if (!movie) {
	console.log("data");
    return <Typography variant="h6" align="center" style={{ padding: "20px" }}>Movie not found</Typography>;
  }

  return (
	
    <Paper style={{ maxWidth: 600, margin: "auto", padding: 20, marginTop: 20 }}>
      <Typography variant="h4" gutterBottom>{movie.title}</Typography>
      <Typography variant="h6">Director: {movie.director}</Typography>
      <Typography variant="h6">Score: {movie.avg_review}</Typography>
      <Typography variant="body1">{movie.description}</Typography>
	  <MovieReviews />
    </Paper>
	
  );
};

export default MovieDetails;
