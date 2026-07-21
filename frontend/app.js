const API = "/api";
let token = localStorage.getItem("hwToken") || "";
let currentUser = localStorage.getItem("hwUser") || "";
let assignments = [];

const loginScreen = document.getElementById("loginScreen");
const dashboardScreen = document.getElementById("dashboardScreen");
const assignmentList = document.getElementById("assignmentList");
const addFormCard = document.getElementById("addFormCard");
const assignmentForm = document.getElementById("assignmentForm");

function showDashboard() {
  loginScreen.classList.add("hidden");
  dashboardScreen.classList.remove("hidden");
  document.getElementById("welcomeText").textContent = `Signed in as ${currentUser}`;
  loadAssignments();
}

function showLogin() {
  dashboardScreen.classList.add("hidden");
  loginScreen.classList.remove("hidden");
}

async function apiRequest(path, options = {}) {
  const headers = options.headers || {};
  headers["Content-Type"] = "application/json";
  if (token) headers["Authorization"] = `Bearer ${token}`;

  const response = await fetch(`${API}${path}`, { ...options, headers });
  if (response.status === 204) return null;

  const data = await response.json();
  if (!response.ok) throw new Error(data.error || "Request failed");
  return data;
}

document.getElementById("loginBtn").addEventListener("click", async () => {
  const email = document.getElementById("emailInput").value.trim();
  const password = document.getElementById("passwordInput").value;
  const message = document.getElementById("loginMessage");

  try {
    const data = await apiRequest("/login", {
      method: "POST",
      body: JSON.stringify({ email, password })
    });

    token = data.token;
    currentUser = data.user.email;
    localStorage.setItem("hwToken", token);
    localStorage.setItem("hwUser", currentUser);
    message.textContent = "";
    showDashboard();
  } catch (error) {
    message.textContent = error.message;
  }
});

document.getElementById("logoutBtn").addEventListener("click", () => {
  token = "";
  currentUser = "";
  localStorage.removeItem("hwToken");
  localStorage.removeItem("hwUser");
  showLogin();
});

document.getElementById("showAddFormBtn").addEventListener("click", () => {
  resetForm();
  addFormCard.classList.remove("hidden");
});

document.getElementById("cancelFormBtn").addEventListener("click", () => {
  resetForm();
  addFormCard.classList.add("hidden");
});

document.getElementById("filterSelect").addEventListener("change", renderAssignments);

assignmentForm.addEventListener("submit", async (event) => {
  event.preventDefault();

  const id = document.getElementById("assignmentId").value;
  const payload = {
    title: document.getElementById("titleInput").value.trim(),
    course: document.getElementById("courseInput").value.trim(),
    dueDate: document.getElementById("dueDateInput").value,
    notes: document.getElementById("notesInput").value.trim(),
    reminderEnabled: document.getElementById("reminderEnabledInput").checked,
    reminderTime: document.getElementById("reminderTimeInput").value
  };

  try {
    if (id) {
      await apiRequest(`/assignments/${id}`, {
        method: "PATCH",
        body: JSON.stringify(payload)
      });
    } else {
      await apiRequest("/assignments", {
        method: "POST",
        body: JSON.stringify(payload)
      });
    }

    resetForm();
    addFormCard.classList.add("hidden");
    await loadAssignments();
  } catch (error) {
    document.getElementById("formMessage").textContent = error.message;
  }
});

async function loadAssignments() {
  try {
    const data = await apiRequest("/assignments");
    assignments = data.assignments;
    renderAssignments();
  } catch (error) {
    assignmentList.innerHTML = `<p>${error.message}</p>`;
  }
}

function renderAssignments() {
  const filter = document.getElementById("filterSelect").value;
  let visible = assignments;

  if (filter === "active") visible = assignments.filter(a => !a.completed);
  if (filter === "completed") visible = assignments.filter(a => a.completed);

  if (visible.length === 0) {
    assignmentList.innerHTML = "<p>No assignments found.</p>";
    return;
  }

  assignmentList.innerHTML = visible.map(a => `
    <article class="assignment-card ${a.completed ? "completed" : ""}">
      <h3>${escapeHtml(a.title)}</h3>
      <p><strong>Course:</strong> ${escapeHtml(a.course || "Not specified")}</p>
      <p><strong>Due:</strong> ${escapeHtml(a.dueDate)}</p>
      <p><strong>Notes:</strong> ${escapeHtml(a.notes || "None")}</p>
      <p><strong>Reminder:</strong> ${a.reminderEnabled ? escapeHtml(a.reminderTime) : "Off"}</p>
      <p><strong>Status:</strong> ${a.completed ? "Completed" : "Not Complete"}</p>
      <div class="assignment-actions">
        <button onclick="editAssignment('${a.id}')">Edit</button>
        <button onclick="toggleComplete('${a.id}', ${!a.completed})">
          ${a.completed ? "Mark Incomplete" : "Mark Complete"}
        </button>
        <button class="secondary" onclick="deleteAssignment('${a.id}')">Delete</button>
      </div>
    </article>
  `).join("");
}

function editAssignment(id) {
  const a = assignments.find(item => item.id === id);
  if (!a) return;

  document.getElementById("assignmentId").value = a.id;
  document.getElementById("titleInput").value = a.title;
  document.getElementById("courseInput").value = a.course;
  document.getElementById("dueDateInput").value = a.dueDate;
  document.getElementById("notesInput").value = a.notes;
  document.getElementById("reminderEnabledInput").checked = a.reminderEnabled;
  document.getElementById("reminderTimeInput").value = a.reminderTime;
  addFormCard.classList.remove("hidden");
}

async function toggleComplete(id, completed) {
  await apiRequest(`/assignments/${id}`, {
    method: "PATCH",
    body: JSON.stringify({ completed })
  });
  await loadAssignments();
}

async function deleteAssignment(id) {
  if (!confirm("Delete this assignment?")) return;
  await apiRequest(`/assignments/${id}`, { method: "DELETE" });
  await loadAssignments();
}

function resetForm() {
  assignmentForm.reset();
  document.getElementById("assignmentId").value = "";
  document.getElementById("formMessage").textContent = "";
}

function escapeHtml(value) {
  return String(value)
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;")
    .replaceAll("'", "&#039;");
}

if (token && currentUser) showDashboard();
else showLogin();
