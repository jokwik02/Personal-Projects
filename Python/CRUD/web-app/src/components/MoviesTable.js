import React, { useEffect, useState } from "react";
import { useNavigate } from "react-router-dom";
import { Table, TableBody, TableCell, TableContainer, TableHead, TableRow, Paper, CircularProgress, Typography } from "@mui/material";
import API_BASE_URL from "../config";

const MoviesTable = () => {
  const [movies, setMovies] = useState([]);
  const [loading, setLoading] = useState(true);
  const navigate = useNavigate(); // Hook for navigation

  useEffect(() => {
    fetch(`${API_BASE_URL}/movies`) // Adjust to your Flask server address
      .then((response) => response.json())
      .then((data) => {
        setMovies(data);
        setLoading(false);
      })
      .catch((error) => {
        console.error("Error fetching movies:", error);
        setLoading(false);
      });
  }, []);

  if (loading) {
    return <CircularProgress style={{ display: "block", margin: "20px auto" }} />;
  }

  return (
    <TableContainer component={Paper} style={{ maxWidth: 800, margin: "auto", marginTop: 20 }}>
      <Typography variant="h6" align="center" style={{ padding: "10px 0" }}>
        Movies List
      </Typography>
      <Table>
        <TableHead>
          <TableRow>
            <TableCell><strong>Title</strong></TableCell>
            <TableCell><strong>Director</strong></TableCell>
			<TableCell><strong>Year</strong></TableCell>
            <TableCell align="right"><strong>Score</strong></TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {movies.map((movie, index) => (
            <TableRow 
              key={index} 
              hover
              style={{ cursor: "pointer" }}
              onClick={() => navigate(`/movies/${movie.id}`)} // Navigate on click
            >
              <TableCell>{movie.title}</TableCell>
              <TableCell>{movie.director}</TableCell>
			  <TableCell>{movie.year}</TableCell>
              <TableCell align="right">{movie.avg_review}</TableCell>
            </TableRow>
          ))}
        </TableBody>
      </Table>
    </TableContainer>
  );
};

export default MoviesTable;
