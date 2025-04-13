import React from 'react';

const AboutPopup = ({ onClose }) => {
    return (
        <div className="modal">
            <div className="modal-content">
                <h2>About DriveHub</h2>
                <p>
                    <strong>DriveHub</strong> is the ultimate all-in-one platform designed to revolutionize the way car dealerships 
                    operate. Whether you're managing a small dealership or a large-scale enterprise, DriveHub provides 
                    seamless tools to optimize inventory tracking, streamline sales processes, and enhance customer 
                    relationships.
                </p>
                <h3>Key Features:</h3>
                <ul>
                    <li><strong>Real-Time Inventory Management:</strong> Easily add, remove, and update vehicle details while tracking stock availability.</li>
                    <li><strong>Automated Sales & Customer Records:</strong> Keep a well-organized database of sales transactions, customer details, and lead management.</li>
                    <li><strong>Advanced Search & Filtering:</strong> Quickly find cars based on make, model, price, and other parameters to boost efficiency.</li>
                    <li><strong>Customizable Reporting:</strong> Generate detailed reports on sales, inventory, and customer interactions for data-driven decisions.</li>
                    <li><strong>Secure & Cloud-Based:</strong> Access your dealership data from anywhere, backed by top-tier security.</li>
                    <li><strong>Seamless User Experience:</strong> Designed for  dealership owners , ensuring an intuitive and engaging experience.</li>
                </ul>
                <p>
                    With <strong>DriveHub</strong>, you can eliminate manual processes, reduce operational costs, and focus on what truly 
                    matters growing your business and delivering exceptional customer service. Join us today and take 
                    your dealership to the next level!
                </p>
                <button onClick={onClose}>Close</button>
            </div>
        </div>
    );
};

export default AboutPopup;
