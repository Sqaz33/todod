#include "script_repository.hpp"

#include <format>
#include <utility>

namespace {

const std::string ID_COLUMN = "id";
const std::string NAME_COLUMN = "name";
const std::string SOURCE_COLUMN = "source";
const std::string EVENT_COLUMN = "event";
const std::string ENABLED_COLUMN = "enabled";
const std::string TABLE_NAME = "scripts";

todod::HandlerScript readScript(SQLite::Statement& query) {
    return todod::HandlerScript{
        query.getColumn(0).getInt64(),
        query.getColumn(1).getString(),
        query.getColumn(2).getString(),
        static_cast<todod::TodoEvent>(query.getColumn(3).getInt()),
        query.getColumn(4).getInt() != 0};
}

} // namespace

namespace todod::repository {

ScriptRepository::ScriptRepository(std::shared_ptr<db::DataBase> db) :
    db_(std::move(db)),
    insertionQuery_(db_->connection(), std::format(
        "INSERT INTO {} ({}, {}, {}, {}) VALUES (?, ?, ?, ?)",
        TABLE_NAME,
        NAME_COLUMN,
        SOURCE_COLUMN,
        EVENT_COLUMN,
        ENABLED_COLUMN)),
    getAllQuery_(db_->connection(), std::format(
        "SELECT {}, {}, {}, {}, {} FROM {}",
        ID_COLUMN,
        NAME_COLUMN,
        SOURCE_COLUMN,
        EVENT_COLUMN,
        ENABLED_COLUMN,
        TABLE_NAME)),
    updateQuery_(db_->connection(), std::format(
        R"(
            UPDATE {}
            SET {} = ?,
                {} = ?,
                {} = ?,
                {} = ?
            WHERE {} = ?
        )",
        TABLE_NAME,
        NAME_COLUMN,
        SOURCE_COLUMN,
        EVENT_COLUMN,
        ENABLED_COLUMN,
        ID_COLUMN)),
    findByIdQuery_(db_->connection(), std::format(
        "SELECT {}, {}, {}, {}, {} FROM {} WHERE {} = ?",
        ID_COLUMN,
        NAME_COLUMN,
        SOURCE_COLUMN,
        EVENT_COLUMN,
        ENABLED_COLUMN,
        TABLE_NAME,
        ID_COLUMN)),
    removeQuery_(db_->connection(), std::format(
        "DELETE FROM {} WHERE {} = ?",
        TABLE_NAME,
        ID_COLUMN))
{}

HandlerScript ScriptRepository::create(
    std::string name,
    std::string source,
    TodoEvent event,
    bool enabled)
{
    insert_(name, source, static_cast<int>(event), enabled);

    return HandlerScript{
        db_->connection().getLastInsertRowid(),
        std::move(name),
        std::move(source),
        event,
        enabled};
}

std::vector<HandlerScript> ScriptRepository::getAll() {
    std::vector<HandlerScript> scripts;

    while (getAllQuery_.executeStep()) {
        scripts.emplace_back(readScript(getAllQuery_));
    }

    getAllQuery_.reset();
    getAllQuery_.clearBindings();
    return scripts;
}

bool ScriptRepository::updateTodo(const HandlerScript& script) {
    updateQuery_.bind(1, script.name);
    updateQuery_.bind(2, script.source);
    updateQuery_.bind(3, static_cast<int>(script.event));
    updateQuery_.bind(4, script.enabled);
    updateQuery_.bind(5, script.id);
    updateQuery_.exec();

    const bool updated = db_->connection().getChanges() != 0;

    updateQuery_.reset();
    updateQuery_.clearBindings();
    return updated;
}

std::optional<HandlerScript> ScriptRepository::findByID(std::int64_t id) {
    findByIdQuery_.bind(1, id);

    if (!findByIdQuery_.executeStep()) {
        findByIdQuery_.reset();
        findByIdQuery_.clearBindings();
        return std::nullopt;
    }

    auto script = readScript(findByIdQuery_);

    findByIdQuery_.reset();
    findByIdQuery_.clearBindings();
    return script;
}

bool ScriptRepository::removeTodo(std::int64_t id) {
    removeQuery_.bind(1, id);
    removeQuery_.exec();

    const bool removed = db_->connection().getChanges() != 0;

    removeQuery_.reset();
    removeQuery_.clearBindings();
    return removed;
}

} // namespace todod::repository