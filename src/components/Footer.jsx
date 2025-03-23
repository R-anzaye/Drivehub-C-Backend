import React from "react";
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faInstagram, faLinkedin, faWhatsapp } from '@fortawesome/free-brands-svg-icons';
function Footer (){


    return (
        <>
        <footer>
        <div className="footer-top">
          <p>DriveHub</p>
          <p>To get in touch with us, click on the links below</p>
          <div className="social">
            <a href="#" className="social-link">
              <FontAwesomeIcon icon={faInstagram} />
            </a>
            <a href="#" className="social-link">
              <FontAwesomeIcon icon={faLinkedin} />
            </a>
            <a href="#" className="social-link">
              <FontAwesomeIcon icon={faWhatsapp} />
            </a>
          </div>
        </div>
      
        <div className="footer-middle">
          <div className="footer-column">
            <h3>About Us</h3>
            <p>DriveHub , The ultimate trusted partner to manage your  car dealership</p>
          </div>
          <div className="footer-column">
            <h3>Important Links</h3>
            <ul>
              <li><a href="#">Terms & Conditions</a></li>
              <li><a href="#">Privacy Policy</a></li>
              <li><a href="#">FAQ</a></li>
            </ul>
          </div>
          <div className="footer-column">
            <h3>Contact Us</h3>
            <p>Email: support@drivehub.com</p>
            <p>Phone: +254 705 133 012</p>
          </div>
        </div>
      
        <div className="footer-bottom">
          <p>&copy; 2024 DriveHub. All rights reserved.</p>
        </div>
      </footer>
      </>
    )
}
export default Footer