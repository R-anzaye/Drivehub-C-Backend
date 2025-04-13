import React,{useState} from "react";
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faBars, faHome, faBell, faBox, faChartBar, faCog, faUser } from '@fortawesome/free-solid-svg-icons';
function Navbar(){
    const [isSidebarExpanded, setSidebarExpanded] = useState(false);

    const toggleSidebar = () => {
      setSidebarExpanded(!isSidebarExpanded)
    }
return(
    <>
 <aside className={`sidebar ${isSidebarExpanded ? 'expanded' : 'collapsed'}`}>
<button onClick={toggleSidebar} className="toggle-btn">
  <FontAwesomeIcon icon={isSidebarExpanded ? faBars : faBars} />
</button>
<div className={`logo ${isSidebarExpanded ? '' : 'collapsed'}`}>
  <h2>DriveHub</h2>
</div>
<nav className="nav">
  <ul>
    <li>
      <a href="/" className={isSidebarExpanded ? 'active' : ''}>
        <FontAwesomeIcon icon={faHome} className="icon" />
        {isSidebarExpanded && 'Home'}
      </a>
    </li>
    <li>
      <a href="notification">
        <FontAwesomeIcon icon={faBell} className="icon" />
        {isSidebarExpanded && 'Notifications'}
      </a>
    </li>
    <li>
      <a href="newCar">
        <FontAwesomeIcon icon={faBox} className="icon" />
        {isSidebarExpanded && 'NewCar'}
      </a>
    </li>
    <li>
      <a href="reports">
        <FontAwesomeIcon icon={faChartBar} className="icon" />
        {isSidebarExpanded && 'Reports'}
      </a>
    </li>
    <li>
      <a href="/settings">
        <FontAwesomeIcon icon={faCog} className="icon" />
        {isSidebarExpanded && 'Settings'}
      </a>
    </li>
    <li>
      <a href="/profile">
        <FontAwesomeIcon icon={faUser} className="icon" />
        {isSidebarExpanded && 'Profile'}
      </a>
    </li>
  </ul>
</nav>
</aside>
</>

)}
export default Navbar