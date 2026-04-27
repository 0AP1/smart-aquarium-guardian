/**
 * Aquarium Management System
 * Core Script for Login and Dashboard
 */

document.addEventListener('DOMContentLoaded', () => {
    // 1. Background Bubble Animation
    createBubbles();

    // 2. Login Logic (if on index.html)
    const loginForm = document.getElementById('login-form');
    if (loginForm) {
        handleLogin(loginForm);
    }

    // 3. Dashboard Logic (if on dashboard.html)
    const liveClock = document.getElementById('live-clock');
    if (liveClock) {
        updateClock();
        setInterval(updateClock, 1000);
        animateProgressBars();
    }

    // 4. Logout Logic
    const logoutBtn = document.getElementById('logout-btn');
    if (logoutBtn) {
        logoutBtn.addEventListener('click', () => {
            window.location.href = 'index.html';
        });
    }
});

/**
 * Creates floating bubbles for the background
 */
function createBubbles() {
    const container = document.getElementById('bubbles-container');
    if (!container) return;

    const bubbleCount = 20;
    for (let i = 0; i < bubbleCount; i++) {
        const bubble = document.createElement('div');
        bubble.className = 'bubble';

        // Randomize size, position, and animation timing
        const size = Math.random() * 30 + 10 + 'px';
        bubble.style.width = size;
        bubble.style.height = size;
        bubble.style.left = Math.random() * 100 + '%';
        bubble.style.animationDuration = Math.random() * 10 + 5 + 's';
        bubble.style.animationDelay = Math.random() * 5 + 's';

        container.appendChild(bubble);
    }
}

/**
 * Handles the login form submission
 */
function handleLogin(form) {
    const loginBtn = document.getElementById('login-btn');
    const btnLoader = document.getElementById('btn-loader');
    const btnText = document.getElementById('btn-text');
    const errorMessage = document.getElementById('error-message');
    const loginCard = document.getElementById('login-card');

    form.addEventListener('submit', (e) => {
        e.preventDefault();

        const email = document.getElementById('email').value;
        const password = document.getElementById('password').value;

        // Reset state
        errorMessage.style.opacity = '0';
        loginCard.classList.remove('shake');

        // Show loading animation
        btnLoader.style.display = 'block';
        btnText.style.visibility = 'hidden';
        loginBtn.disabled = true;

        // Mock a network delay for better UX
        setTimeout(() => {
            if (email === 'admin@gmail.com' && password === 'Qwerty123$') {
                window.location.href = 'dashboard.html';
            } else {
                // Failure
                btnLoader.style.display = 'none';
                btnText.style.visibility = 'visible';
                loginBtn.disabled = false;

                errorMessage.style.opacity = '1';
                loginCard.classList.add('shake');

                // Clear password field
                document.getElementById('password').value = '';
            }
        }, 1500);
    });
}

/**
 * Updates the live clock on the dashboard
 */
function updateClock() {
    const clockElement = document.getElementById('live-clock');
    if (!clockElement) return;

    const now = new Date();
    const timeString = now.toLocaleTimeString([], { hour12: true });
    clockElement.textContent = timeString;
}

/**
 * Animates progress bars on dashboard load
 */
function animateProgressBars() {
    const bars = document.querySelectorAll('.progress-bar');
    // The widths are already set in HTML, but we can trigger the transition
    // by momentarily setting them to 0 and back if we wanted, 
    // but the CSS transition will handle it on initial render if we delay slightly.
    setTimeout(() => {
        bars.forEach(bar => {
            const width = bar.style.width;
            bar.style.width = '0';
            setTimeout(() => {
                bar.style.width = width;
            }, 100);
        });
    }, 500);
}


// CLOCK
function updateClock() {
    const clock = document.getElementById("live-clock");
    if (!clock) return;

    const now = new Date();
    clock.textContent = now.toLocaleTimeString();
}
setInterval(updateClock, 1000);

// NOTIFICATION SYSTEM
const notificationBtn = document.getElementById("notification-btn");
const dropdown = document.getElementById("notification-dropdown");
const list = document.getElementById("notification-list");
const badge = document.getElementById("notification-count");

let notifications = [];

// Toggle dropdown
notificationBtn.addEventListener("click", () => {
    dropdown.style.display =
        dropdown.style.display === "flex" ? "none" : "flex";
});

// Add notification
function addNotification(message) {
    notifications.unshift(message);

    const li = document.createElement("li");
    li.textContent = message;
    list.prepend(li);

    badge.textContent = notifications.length;
}

// NEXT FEEDING NOTIFICATION
function checkFeeding() {
    const now = new Date();
    const currentHour = now.getHours();

    // Example: Feeding at 18:00 (6 PM)
    if (currentHour === 17) {
        addNotification("⏰ Feeding time in 1 hour");
    }
}

// OXYGEN CHECK
function checkOxygen() {
    const oxygenValue = 92; // replace with dynamic later

    if (oxygenValue < 85) {
        addNotification("⚠️ Oxygen level is LOW!");
        
        // Browser Notification
        if (Notification.permission === "granted") {
            new Notification("Aquarium Alert", {
                body: "Oxygen level dropped below 85%",
            });
        }
    }
}

// REQUEST PERMISSION
if ("Notification" in window) {
    Notification.requestPermission();
}

// RUN CHECKS
setInterval(checkFeeding, 60000);
setInterval(checkOxygen, 5000);

// DEMO INITIAL NOTIFICATION
addNotification("✅ System running smoothly");