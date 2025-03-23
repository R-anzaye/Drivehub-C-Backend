import React, { useState, useEffect } from 'react';
import { useAuth } from '../context/AuthContext'; // Import the custom hook

const UpdateEmailNamePopup = ({ onClose }) => {
    const { user, token, updateProfile } = useAuth(); // Destructure the user, token, and updateProfile from the custom hook
    const [email, setEmail] = useState('');
    const [error, setError] = useState('');
    const [loading, setLoading] = useState(false);

    useEffect(() => {
        // Ensure the email state is updated only if the user object is available
        if (user && user.email) {
            setEmail(user.email);
        }
    }, [user]);

    const handleSave = async (e) => {
        e.preventDefault();
        setLoading(true);
        setError('');

        try {
            // Call updateProfile from useAuth to update the user's email
            await updateProfile({ email });

            alert('Email updated successfully.');
            onClose();  // Close the modal after the update
        } catch (error) {
            setError('An error occurred while updating the email.');
        } finally {
            setLoading(false);
        }
    };

    return (
        <div className="modal">
            <div className="modal-content">
                <h2>Update Email</h2>
                {error && <p className="error">{error}</p>}
                <form onSubmit={handleSave}>
                    <input
                        type="email"
                        value={email}
                        onChange={(e) => setEmail(e.target.value)}
                        placeholder="New Email"
                        required
                        disabled={!user} // Disable the input if the user object is not loaded
                    />
                    <button type="submit" disabled={loading || !user}>Save</button>
                    <button type="button" onClick={onClose}>Cancel</button>
                </form>
            </div>
        </div>
    );
};

export default UpdateEmailNamePopup;
