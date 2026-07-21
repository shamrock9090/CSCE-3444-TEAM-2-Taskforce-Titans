#include <httplib.h>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using json = nlohmann::json;
namespace fs = std::filesystem;

struct Assignment {
    std::string id;
    std::string userId;
    std::string title;
    std::string course;
    std::string dueDate;
    std::string notes;
    bool reminderEnabled = false;
    std::string reminderTime = "24 hours";
    bool completed = false;
};

void to_json(json& j, const Assignment& a) {
    j = {
        {"id", a.id},
        {"userId", a.userId},
        {"title", a.title},
        {"course", a.course},
        {"dueDate", a.dueDate},
        {"notes", a.notes},
        {"reminderEnabled", a.reminderEnabled},
        {"reminderTime", a.reminderTime},
        {"completed", a.completed}
    };
}

void from_json(const json& j, Assignment& a) {
    j.at("id").get_to(a.id);
    j.at("userId").get_to(a.userId);
    j.at("title").get_to(a.title);
    j.at("course").get_to(a.course);
    j.at("dueDate").get_to(a.dueDate);
    a.notes = j.value("notes", "");
    a.reminderEnabled = j.value("reminderEnabled", false);
    a.reminderTime = j.value("reminderTime", "24 hours");
    a.completed = j.value("completed", false);
}

class Repository {
public:
    explicit Repository(fs::path path) : path_(std::move(path)) {
        load();
    }

    std::vector<Assignment> list(const std::string& userId) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Assignment> result;
        for (const auto& a : assignments_) {
            if (a.userId == userId) result.push_back(a);
        }

        std::sort(result.begin(), result.end(),
            [](const Assignment& a, const Assignment& b) {
                return a.dueDate < b.dueDate;
            });

        return result;
    }

    Assignment create(Assignment a) {
        std::lock_guard<std::mutex> lock(mutex_);
        a.id = createId();
        assignments_.push_back(a);
        save();
        return a;
    }

    std::optional<Assignment> update(
        const std::string& id,
        const std::string& userId,
        const json& patch
    ) {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = std::find_if(assignments_.begin(), assignments_.end(),
            [&](const Assignment& a) {
                return a.id == id && a.userId == userId;
            });

        if (it == assignments_.end()) return std::nullopt;

        if (patch.contains("title")) it->title = patch["title"].get<std::string>();
        if (patch.contains("course")) it->course = patch["course"].get<std::string>();
        if (patch.contains("dueDate")) it->dueDate = patch["dueDate"].get<std::string>();
        if (patch.contains("notes")) it->notes = patch["notes"].get<std::string>();
        if (patch.contains("reminderEnabled")) {
            it->reminderEnabled = patch["reminderEnabled"].get<bool>();
        }
        if (patch.contains("reminderTime")) {
            it->reminderTime = patch["reminderTime"].get<std::string>();
        }
        if (patch.contains("completed")) {
            it->completed = patch["completed"].get<bool>();
        }

        save();
        return *it;
    }

    bool remove(const std::string& id, const std::string& userId) {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto oldSize = assignments_.size();

        assignments_.erase(
            std::remove_if(assignments_.begin(), assignments_.end(),
                [&](const Assignment& a) {
                    return a.id == id && a.userId == userId;
                }),
            assignments_.end()
        );

        if (assignments_.size() == oldSize) return false;
        save();
        return true;
    }

private:
    fs::path path_;
    std::vector<Assignment> assignments_;
    std::mutex mutex_;

    static std::string createId() {
        static std::mt19937_64 rng(std::random_device{}());
        std::ostringstream out;
        out << std::hex << rng();
        return out.str();
    }

    void load() {
        fs::create_directories(path_.parent_path());

        if (!fs::exists(path_)) {
            save();
            return;
        }

        std::ifstream input(path_);
        json j;
        input >> j;
        assignments_ = j.value("assignments", std::vector<Assignment>{});
    }

    void save() {
        fs::create_directories(path_.parent_path());
        std::ofstream output(path_);
        output << json{{"assignments", assignments_}}.dump(2);
    }
};

void setJson(httplib::Response& res, int status, const json& body) {
    res.status = status;
    res.set_content(body.dump(2), "application/json");
}

void addCors(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PATCH, DELETE, OPTIONS");
}

std::optional<std::string> authenticate(const httplib::Request& req) {
    const std::string value = req.get_header_value("Authorization");
    if (value == "Bearer demo-token") return std::string("student-001");
    return std::nullopt;
}

bool validDate(const std::string& date) {
    return date.size() == 10 && date[4] == '-' && date[7] == '-';
}

int main() {
    Repository repository("data/assignments.json");
    httplib::Server server;

    server.set_pre_routing_handler(
        [](const httplib::Request&, httplib::Response& res) {
            addCors(res);
            return httplib::Server::HandlerResponse::Unhandled;
        }
    );

    server.Options(R"(.*)",
        [](const httplib::Request&, httplib::Response& res) {
            res.status = 204;
        }
    );

    server.Get("/api/health",
        [](const httplib::Request&, httplib::Response& res) {
            setJson(res, 200, {{"status", "ok"}});
        }
    );

    server.Post("/api/login",
        [](const httplib::Request& req, httplib::Response& res) {
            try {
                const json body = json::parse(req.body);
                const std::string email = body.value("email", "");
                const std::string password = body.value("password", "");

                if (email.empty() || password.empty()) {
                    setJson(res, 400, {{"error", "Email and password are required."}});
                    return;
                }

                setJson(res, 200, {
                    {"token", "demo-token"},
                    {"user", {
                        {"id", "student-001"},
                        {"email", email}
                    }}
                });
            } catch (...) {
                setJson(res, 400, {{"error", "Invalid login request."}});
            }
        }
    );

    server.Get("/api/assignments",
        [&](const httplib::Request& req, httplib::Response& res) {
            auto userId = authenticate(req);
            if (!userId) {
                setJson(res, 401, {{"error", "Unauthorized."}});
                return;
            }

            setJson(res, 200, {
                {"assignments", repository.list(*userId)}
            });
        }
    );

    server.Post("/api/assignments",
        [&](const httplib::Request& req, httplib::Response& res) {
            auto userId = authenticate(req);
            if (!userId) {
                setJson(res, 401, {{"error", "Unauthorized."}});
                return;
            }

            try {
                json body = json::parse(req.body);
                std::string title = body.value("title", "");
                std::string dueDate = body.value("dueDate", "");

                if (title.empty() || dueDate.empty()) {
                    setJson(res, 400, {{"error", "Title and due date are required."}});
                    return;
                }

                if (!validDate(dueDate)) {
                    setJson(res, 400, {{"error", "Due date must use YYYY-MM-DD."}});
                    return;
                }

                Assignment a;
                a.userId = *userId;
                a.title = title;
                a.course = body.value("course", "");
                a.dueDate = dueDate;
                a.notes = body.value("notes", "");
                a.reminderEnabled = body.value("reminderEnabled", false);
                a.reminderTime = body.value("reminderTime", "24 hours");
                a.completed = body.value("completed", false);

                setJson(res, 201, repository.create(a));
            } catch (...) {
                setJson(res, 400, {{"error", "Invalid assignment data."}});
            }
        }
    );

    server.Patch(R"(/api/assignments/([A-Za-z0-9]+))",
        [&](const httplib::Request& req, httplib::Response& res) {
            auto userId = authenticate(req);
            if (!userId) {
                setJson(res, 401, {{"error", "Unauthorized."}});
                return;
            }

            try {
                json patch = json::parse(req.body);

                if (patch.contains("title") &&
                    patch["title"].get<std::string>().empty()) {
                    setJson(res, 400, {{"error", "Title cannot be empty."}});
                    return;
                }

                if (patch.contains("dueDate") &&
                    !validDate(patch["dueDate"].get<std::string>())) {
                    setJson(res, 400, {{"error", "Due date must use YYYY-MM-DD."}});
                    return;
                }

                auto updated = repository.update(
                    req.matches[1].str(), *userId, patch
                );

                if (!updated) {
                    setJson(res, 404, {{"error", "Assignment not found."}});
                    return;
                }

                setJson(res, 200, *updated);
            } catch (...) {
                setJson(res, 400, {{"error", "Invalid update request."}});
            }
        }
    );

    server.Delete(R"(/api/assignments/([A-Za-z0-9]+))",
        [&](const httplib::Request& req, httplib::Response& res) {
            auto userId = authenticate(req);
            if (!userId) {
                setJson(res, 401, {{"error", "Unauthorized."}});
                return;
            }

            if (!repository.remove(req.matches[1].str(), *userId)) {
                setJson(res, 404, {{"error", "Assignment not found."}});
                return;
            }

            res.status = 204;
        }
    );

    std::cout << "Backend running on http://localhost:8080\n";
    std::cout << "Open frontend/index.html in your browser.\n";

    if (!server.listen("0.0.0.0", 8080)) {
        std::cerr << "Could not start server.\n";
        return 1;
    }

    return 0;
}
