import React, { useEffect, useState, useContext } from "react";
import { useParams } from "react-router-dom";
import { Card, CardContent, Typography, CircularProgress, Box, Paper, Button, Dialog, DialogTitle, DialogContent, DialogActions } from "@mui/material";
import { AuthContext } from "../context/AuthContext";
import ReviewForm from "./ReviewForm";
import API_BASE_URL from "../config";

const MovieReviews = () => {
  const { id } = useParams(); // Get movie ID from URL
  const [reviews, setReviews] = useState([]);
  const [loading, setLoading] = useState(true);
  const { user } = useContext(AuthContext); // Get logged-in user
  const [editingReview, setEditingReview] = useState(null); // Track review being edited
  const [deleteReviewId, setDeleteReviewId] = useState(null); // Track review to delete

  // Function to fetch latest reviews
  const fetchReviews = async () => {
    try {
      const response = await fetch(`${API_BASE_URL}/movies/reviews/${id}`);
      const data = await response.json();
      console.log("Fetched latest reviews:", data); // Debugging
      setReviews(data);
      setLoading(false);
    } catch (error) {
      console.error("Error fetching reviews:", error);
      setLoading(false);
    }
  };

  // Fetch reviews when the component mounts
  useEffect(() => {
    fetchReviews();
  }, [id]);

  const handleDeleteReview = async () => {
    const token = localStorage.getItem("token");

    if (!token || deleteReviewId === null) {
      console.error("No token found or review ID missing.");
      return;
    }

    const response = await fetch(`${API_BASE_URL}/delete_review/${deleteReviewId}`, {
      method: "DELETE",
      headers: {
        Authorization: `Bearer ${token}`,
      },
    });

    if (response.ok) {
      fetchReviews(); // Refresh reviews after deletion
      setDeleteReviewId(null); // Close confirmation dialog
    } else {
      console.error("Failed to delete review");
    }
  };

  return (
    <Paper style={{ maxWidth: 600, margin: "auto", padding: 20, marginTop: 20 }}>
      <Typography variant="h5" gutterBottom align="center">
        Reviews
      </Typography>

      {/* "Write a Review" button */}
      {user && (
        <Box sx={{ mb: 2, textAlign: "center" }}>
          <Button
            variant="contained"
            color="primary"
            onClick={() => setEditingReview({ review: "", id: null })}
          >
            Write a Review
          </Button>
        </Box>
      )}

      {/* Show the edit form when a review is being edited or a new one is being added */}
      {editingReview && (
        <ReviewForm
          reviewId={editingReview.id}
          review={editingReview}
          movieId={id} // Pass movieId to ReviewForm
          onClose={() => setEditingReview(null)}
          onReviewUpdated={() => {
            console.log("Fetching latest reviews after submission..."); // Debugging
            fetchReviews(); // Ensure latest reviews are fetched after adding/editing a review
          }}
        />
      )}

      {loading ? (
        <CircularProgress style={{ display: "block", margin: "20px auto" }} />
      ) : reviews.length === 0 ? (
        <Typography variant="body1" align="center">No reviews available.</Typography>
      ) : (
        reviews.map((review) => (
          <Card key={review.id} variant="outlined" style={{ marginBottom: 10 }}>
            <CardContent>
              <Typography variant="h6">{review.username}</Typography>
              <Typography variant="body2" color="textSecondary">
                {new Date(review.last_updated).toLocaleDateString()}
              </Typography>
              <Typography variant="body1" style={{ marginTop: 5 }}>
                {review.review}
              </Typography>
              <Typography variant="subtitle2" color="primary">
                Score: {review.rating}/10
              </Typography>

              {/* Show Edit & Delete buttons only if the logged-in user wrote this review */}
              {user && user.username === review.username && (
                <Box sx={{ mt: 1, display: "flex", gap: 1 }}>
                  <Button
                    variant="outlined"
                    onClick={() => setEditingReview(review)}
                  >
                    Edit
                  </Button>
                  <Button
                    variant="outlined"
                    color="error"
                    onClick={() => setDeleteReviewId(review.id)} // Open confirmation dialog
                  >
                    Delete
                  </Button>
                </Box>
              )}
            </CardContent>
          </Card>
        ))
      )}

      {/* Confirmation Dialog for Deleting Review */}
      <Dialog open={deleteReviewId !== null} onClose={() => setDeleteReviewId(null)}>
        <DialogTitle>Confirm Deletion</DialogTitle>
        <DialogContent>
          <Typography>Are you sure you want to delete this review?</Typography>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setDeleteReviewId(null)}>Cancel</Button>
          <Button onClick={handleDeleteReview} variant="contained" color="error">
            Delete
          </Button>
        </DialogActions>
      </Dialog>
    </Paper>
  );
};

export default MovieReviews;
