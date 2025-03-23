import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faCheckDouble, faPaperPlane } from '@fortawesome/free-solid-svg-icons';
import Navbar from "../components/Navbar.jsx";
import "./styles.css";
import React, { useState, useEffect } from 'react';
import { useAuth } from "../context/AuthContext";

function Notifications() {
    const [unreadNotifications, setUnreadNotifications] = useState([]);
    const [readNotifications, setReadNotifications] = useState([]);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);
    const { token } = useAuth();
    const [message, setMessage] = useState("");

    useEffect(() => {
        fetch('http://127.0.0.1:5555/notifications', {
            method: 'GET',
            headers: {
                'Authorization': `Bearer ${token}`,
            },
        })
        .then(response => response.json())
        .then(data => {
            setUnreadNotifications(data.unread || []);
            setReadNotifications(data.read || []);
            setLoading(false);
        })
        .catch(error => {
            console.error('Error fetching notifications:', error);
            setError('Failed to load notifications. Please try again.');
            setLoading(false);
        });
    }, [token]);

    const handleMarkAsRead = (notificationId) => {
        fetch(`http://127.0.0.1:5555/notifications/${notificationId}/mark_read`, {
            method: 'POST',
            headers: {
                'Authorization': `Bearer ${token}`,
            },
        })
        .then(() => {
            const markedNotification = unreadNotifications.find(n => n.id === notificationId);
            setUnreadNotifications(unreadNotifications.filter(n => n.id !== notificationId));
            setReadNotifications([...readNotifications, markedNotification]);
        })
        .catch(error => {
            console.error('Error marking notification as read:', error);
        });
    };

    const NotificationItem = ({ notification, isUnread }) => (
        <div
            className={`flex items-center justify-between p-4 border-b border-gray-200 ${
                isUnread ? 'bg-green-50' : 'bg-white'
            } hover:bg-gray-50 cursor-pointer`}
            onClick={() => isUnread && handleMarkAsRead(notification.id)}
        >
            <div className="flex-1">
                <p className={`text-sm ${isUnread ? 'font-semibold' : 'font-normal'} text-gray-800`}>
                    {notification.message}
                </p>
                <span className="text-xs text-gray-500">
                    {new Date(notification.timestamp).toLocaleString([], {
                        dateStyle: 'short',
                        timeStyle: 'short',
                    })}
                </span>
            </div>
            {isUnread && (
                <FontAwesomeIcon icon={faCheckDouble} className="text-green-500 ml-2" />
            )}
        </div>
    );

    return (
        <>
            <Navbar />
            <div className="notifications-container mx-auto h-screen flex flex-col bg-[#e9a989] rounded-lg">

                {/* Header */}
                <div className="bg-[#e9a989] p-4 border-b border-[#e9a989] rounded-full ">

                <h1 className="text-3xl font-semibold text-gray-800">Notifications</h1>

                    {unreadNotifications.length > 0 && (
                        <span className="text-sm text-gray-600">
                            {unreadNotifications.length} unread
                        </span>
                    )}
                </div>

                {/* Notification List */}
                <div className="flex-1 overflow-y-auto ">
                    {loading && <p className="text-center text-gray-500 p-4">Loading...</p>}
                    {error && <p className="text-center text-red-500 p-4">{error}</p>}
                    {!loading && !error && (
                        <>
                            {unreadNotifications.length === 0 && readNotifications.length === 0 ? (
                                <p className="text-center text-gray-500 p-4">No notifications yet.</p>
                            ) : (
                                <>
                                    {unreadNotifications.map((notification, index) => (
                                        <NotificationItem
                                            key={index}
                                            notification={notification}
                                            isUnread={true}
                                        />
                                    ))}
                                    {readNotifications.map((notification, index) => (
                                        <NotificationItem
                                            key={index}
                                            notification={notification}
                                            isUnread={false}
                                        />
                                    ))}
                                </>
                            )}
                        </>
                    )}
                </div>

                {/* Optional Message Input */}
                <div className="p-2 bg-[#e9a989] border-t border-gray-200 flex items-center space-x-2 ">
                    <input
                        type="text"
                        className="flex-1 p-2 border rounded-lg bg-gray-50 text-sm focus:outline-none focus:ring-2 focus:ring-teal-500 "
                        placeholder="Type a message..."
                        value={message}
                        onChange={(e) => setMessage(e.target.value)}
                    />
                    <button className="bg-teal-400 text-white p-2 rounded-lg hover:bg-teal-600">
                        <FontAwesomeIcon icon={faPaperPlane} />
                    </button>
                </div>
            </div>
        </>
    );
}

export default Notifications;