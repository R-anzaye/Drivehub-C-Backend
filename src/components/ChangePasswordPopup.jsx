import React, { useState } from 'react';
import { useAuth } from '../context/AuthContext'; 

const ChangePasswordPopup = ({ onClose }) => {
    const { user, token } = useAuth();  // Assuming useAuth provides user details and the token
    const [oldPassword, setOldPassword] = useState('');
    const [newPassword, setNewPassword] = useState('');
    const [confirmPassword, setConfirmPassword] = useState('');
    const [step, setStep] = useState(1); // Tracks step (1: old password verification, 2: new password input)
    const [error, setError] = useState('');
    const [loading, setLoading] = useState(false);

    const API_URL = "http://127.0.0.1:5555/";

    // Handle verifying old password (Step 1)
    const handleFirstStep = async (e) => {
        e.preventDefault();
        setLoading(true);
        setError('');

        try {
            const response = await fetch(`${API_URL}auth/password-check`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'Authorization': `Bearer ${token}`  // Sending token for authentication
                },
                body: JSON.stringify({ password: oldPassword })
            });

            if (!response.ok) {
                throw new Error('Failed to authenticate current password');
            }

            const data = await response.json();
            if (data.msg === 'Login successful') {
                setStep(2); // Proceed to next step
            } else {
                setError('Current password is incorrect.');
            }
        } catch (error) {
            setError('An error occurred while validating the password.');
        } finally {
            setLoading(false);
        }
    };

    // Handle updating the password (Step 2)
    const handleSecondStep = async (e) => {
        e.preventDefault();
        setLoading(true);
        setError('');

        // Check if new passwords match
        if (newPassword !== confirmPassword) {
            setError('New passwords do not match.');
            setLoading(false);
            return;
        }

        try {
            const response = await fetch(`${API_URL}users/${user.id}`, {
                method: 'PUT',
                headers: {
                    'Content-Type': 'application/json',
                    'Authorization': `Bearer ${token}` // Authorizing request with the token
                },
                body: JSON.stringify({ password: newPassword })
            });

            if (!response.ok) {
                throw new Error('Failed to update password');
            }

            alert('Password updated successfully.');
            onClose(); // Close the popup after success
        } catch (error) {
            setError('An error occurred while changing the password.');
        } finally {
            setLoading(false);
        }
    };

    return (
        <div className="modal">
            <div className="modal-content">
                <h2>{step === 1 ? 'Change Password' : 'Update Password'}</h2>
                {error && <p className="error">{error}</p>}
                {step === 1 ? (
                    <form onSubmit={handleFirstStep}>
                        <input
                            type="password"
                            value={oldPassword}
                            onChange={(e) => setOldPassword(e.target.value)}
                            placeholder="Old Password"
                            required
                        />
                        <button type="submit" disabled={loading}>Next</button>
                        <button type="button" onClick={onClose}>Cancel</button>
                    </form>
                ) : (
                    <form onSubmit={handleSecondStep}>
                        <input
                            type="password"
                            value={newPassword}
                            onChange={(e) => setNewPassword(e.target.value)}
                            placeholder="New Password"
                            required
                        />
                        <input
                            type="password"
                            value={confirmPassword}
                            onChange={(e) => setConfirmPassword(e.target.value)}
                            placeholder="Confirm Password"
                            required
                        />
                        <button type="submit" disabled={loading}>Update</button>
                        <button type="button" onClick={onClose}>Cancel</button>
                    </form>
                )}
            </div>
        </div>
    );
};

export default ChangePasswordPopup;
