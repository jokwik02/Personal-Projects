import React, { useState, useContext } from "react";
import { AuthContext } from "../context/AuthContext";
import { TextField, Button, Dialog, DialogTitle, DialogContent, DialogActions, InputLabel, Select, MenuItem } from "@mui/material";
import API_BASE_URL from "../config";

const ReviewForm = ({ reviewId, review, movieId, onClose, onReviewUpdated }) => {
  const { user } = useContext(AuthContext);
  const [text, setText] = useState(review ? review.review : "");
  const [rating, setRating] = useState(review ? review.rating : 10); // Default rating = 10
  const isEditing = Boolean(reviewId); 

  const handleSubmit = async () => {
    const token = localStorage.getItem("token");

    if (!token) {
      console.error("No token found! User is not logged in.");
      return;
    }

    const url = isEditing 
      ? `${API_BASE_URL}/edit_review/${reviewId}` 
      : `${API_BASE_URL}/add_review`;

    const method = isEditing ? "PUT" : "POST";

    const body = isEditing 
      ? { id: reviewId, review: text, rating }  // Include rating when editing
      : { movie_id: movieId, review: text, rating, username: localStorage.getItem("username") }; 

    const response = await fetch(url, {
      method,
      headers: {
        "Content-Type": "application/json",
        Authorization: `Bearer ${token}`,  // Ensure token is included
      },
      body: JSON.stringify(body),
    });

    if (response.status === 403) {
      console.error("403 Forbidden: Token is invalid or missing permissions.");
    }

    if (response.ok) {
      const updatedReview = await response.json();
      onReviewUpdated(updatedReview);
      onClose();
    } else {
      console.error("Failed to submit review");
    }
  };

  return (
    <Dialog open onClose={onClose}>
      <DialogTitle>{isEditing ? "Edit Review" : "Add Review"}</DialogTitle>
      <DialogContent>
        {/* Rating Selection */}
        <InputLabel sx={{ mt: 1 }}>Rating (0-10)</InputLabel>
        <Select
          fullWidth
          value={rating}
          onChange={(e) => setRating(e.target.value)}
        >
          {[...Array(11).keys()].map((num) => (
            <MenuItem key={num} value={num}>
              {num}
            </MenuItem>
          ))}
        </Select>

        {/* Review Text Input */}
        <TextField
          fullWidth
          multiline
          label="Your Review"
          value={text}
          onChange={(e) => setText(e.target.value)}
          sx={{ mt: 2 }}
        />
      </DialogContent>
      <DialogActions>
        <Button onClick={onClose}>Cancel</Button>
        <Button onClick={handleSubmit} variant="contained">
          {isEditing ? "Update" : "Submit"}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default ReviewForm;
