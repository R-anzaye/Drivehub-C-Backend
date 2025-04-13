import React, { useState } from "react";
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faPhone, faQuestion, faLock, faHighlighter, faRightFromBracket } from '@fortawesome/free-solid-svg-icons';
import Navbar from "../components/Navbar.jsx";
import ContactUsPopup from "../components/ContactUsPopup.jsx";
import AboutPopup from "../components/AboutPopup.jsx";
import ChangePasswordPopup from "../components/ChangePasswordPopup.jsx";
import UpdateEmailNamePopup from "../components/UpdateEmailNamePopup.jsx";
import { useAuth } from "../context/AuthContext"; // Import the useAuth hook
import "./settings.css";

function Settings() {
    const [showContactUs, setShowContactUs] = useState(false);
    const [showAbout, setShowAbout] = useState(false);
    const [showChangePassword, setShowChangePassword] = useState(false);
    const [showUpdateEmailName, setShowUpdateEmailName] = useState(false);
    const [showDeleteModal, setShowDeleteModal] = useState(false);
    const [deleteText, setDeleteText] = useState('');
    const { deleteAccount } = useAuth(); // Destructure deleteAccount from Auth context

    const handleDeleteAccount = () => {
        if (deleteText.toLowerCase() === "i want to delete account") {
            deleteAccount(); // Call the deleteAccount function from the Auth context
        } else {
            alert("Please type 'I want to delete account' to confirm.");
        }
    };

    return (
        <>
            <Navbar />
            <div className="container">
                <main className="main-content">
                    <h1 className="settings-title">Settings</h1>
                    <div className="description">
                        <img src="/src/settings-icon.png" alt="Settings Icon"></img>
                        <p>Manage your account preferences and settings. Customize your experience</p>
                    </div>
                    <div className="settings-options">
                        <div className="settings-card" onClick={() => setShowContactUs(true)}>
                            <p>Customer Support</p>
                            <div className="settings-icon">
                                <FontAwesomeIcon icon={faPhone} />
                            </div>
                        </div>
                        <div className="settings-card" onClick={() => setShowAbout(true)}>
                            <p>About DriveHub</p>
                            <div className="settings-icon">
                                <FontAwesomeIcon icon={faQuestion} />
                            </div>
                        </div>
                        <div className="settings-card" onClick={() => setShowChangePassword(true)}>
                            <p>Change Password</p>
                            <div className="settings-icon">
                                <FontAwesomeIcon icon={faLock} />
                            </div>
                        </div>
                        <div className="settings-card" onClick={() => setShowUpdateEmailName(true)}>
                            <p>Update Email</p>
                            <div className="settings-icon">
                                <FontAwesomeIcon icon={faHighlighter} />
                            </div>
                        </div>
                        <div className="settings-card" onClick={() => setShowDeleteModal(true)}>
                            <p>Delete Account</p>
                            <div className="settings-icon">
                                <FontAwesomeIcon icon={faRightFromBracket} />
                            </div>
                        </div>
                    </div>
                </main>
            </div>

            {showContactUs && <ContactUsPopup onClose={() => setShowContactUs(false)} />}
            {showAbout && <AboutPopup onClose={() => setShowAbout(false)} />}
            {showChangePassword && (
                <ChangePasswordPopup
                    onClose={() => setShowChangePassword(false)}
                /> 
            )}
            {showUpdateEmailName && (
                <UpdateEmailNamePopup
                    onClose={() => setShowUpdateEmailName(false)}
                    onSave={(email, firstName, lastName) => console.log(email, firstName, lastName)}
                />
            )}

            {showDeleteModal && (
                <div className="modal">
                    <div className="modal-content">
                        <h2>Delete Account Confirmation</h2>
                        <h3>Please type "I want to delete account" to confirm:</h3>
                        <input
                            type="text"
                            value={deleteText}
                            onChange={(e) => setDeleteText(e.target.value)}
                            placeholder="Type here..."
                        />
                        <button onClick={handleDeleteAccount}>Confirm Deletion</button>
                        <button onClick={() => setShowDeleteModal(false)}>Cancel</button>
                    </div>
                </div>
            )}
        </>
    );
}

export default Settings;
