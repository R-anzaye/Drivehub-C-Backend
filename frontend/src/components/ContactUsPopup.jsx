import React from 'react';
const ContactUsPopup = ({ onClose }) => {
    return (
        <div className="modal">
            <div className="modal-content">
                <h2>Contact Us</h2>
                <p>Phone: +254705133012</p>
                <p>Email: support@drivehub.com</p>
                <p>Instagram: @drivehub</p>
                <p>LinkedIn: DriveHub</p>
                
                <button onClick={onClose}>Close</button>
            </div>
        </div>
    );
};

export default ContactUsPopup;
