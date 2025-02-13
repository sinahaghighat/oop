#include <GAME/engine/GameBoard.hpp>
#include <iostream>
#include <cassert>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <GAME/engine/BoardListener.hpp>


GameBoard::GameBoard(std::istream& is, InObjectsTable& table) {
	ObjectInfo info;

	info = table.readInfo(is);
	if (info.needWrite) {
		view.readObject(is, table);
		table.setObject(info, &view);
	}
	else {
		table.getObject(info, [this](void* object) {
			this->view = *((BoardView*)object);
		});
	}

	assert(Serializer::checkControlPoint(is));

	int childCount;
	Serializer::readInt(is, &childCount);
	for (int i = 0; i < childCount; i++) {
		ObjectInfo info;

		info = table.readInfo(is);
		if (info.needWrite)
			table.setObject(info, GameObject::readObject(is, table));
		table.getObject(info, [this](void* obj) {
			this->children.add(shared_ptr<GameObject>((GameObject*)obj));
		});
	}

	assert(Serializer::checkControlPoint(is));

	Serializer::read(is, &cellsCount, sizeof(sf::Vector2i));
	cells = new Cell**[cellsCount.x];
	for (int i = 0; i < cellsCount.x; i++)
		cells[i] = new Cell*[cellsCount.y];
	for (int x = 0; x < cellsCount.x; x++)
		for (int y = 0; y < cellsCount.y; y++) {
			info = table.readInfo(is);
			if (info.needWrite)
				table.setObject(info, new Cell(is, table, *this));
			table.getObject(info, [this, x, y](void* obj) {
				this->cells[x][y] = (Cell*)obj;
			});
		}

	info = table.readInfo(is);
	if (info.needWrite)
		table.setObject(info, new Cell(is, table, *this));
	table.getObject(info, [this](void* obj) {
		this->trashCell = (Cell*)obj;
	});

	assert(Serializer::checkControlPoint(is));

	info = table.readInfo(is);
	if (info.needWrite) {
		controller.readObject(is, table);
		table.setObject(info, &controller);
	}
	else {
		table.getObject(info, [this](void* obj) {
			this->controller = *((ObjectsController*)obj);
		});
	}

	int listenersCount;
	Serializer::readInt(is, &listenersCount);
	for (int i = 0; i < listenersCount; i++) {
		info = table.readInfo(is);
		assert(!info.needWrite);
		table.getObject(info, [this](void*obj) {
			this->_listeners.add((BoardListener*)obj);
		});
	}

	assert(Serializer::checkControlPoint(is));
}

void GameBoard::writeObject(std::ostream& os, OutObjectsTable& table) {
	ObjectInfo info;

	info = table.writeInfo(os, &view);
	if (info.needWrite) 
		view.writeObject(os, table);

	Serializer::writeControlPoint(os);

	int childCount = children.getSize();
	Serializer::writeInt(os, &childCount);
	for (shared_ptr<GameObject> ptr : children) {
		GameObject* obj = ptr.get();
		info = table.writeInfo(os, obj);
		if (info.needWrite)
			GameObject::writeObject(os, table, obj);
	}

	Serializer::writeControlPoint(os);

	Serializer::write(os, &cellsCount, sizeof(sf::Vector2i));
	for (int x = 0; x < cellsCount.x; x++)
		for (int y = 0; y < cellsCount.y; y++) {
			info = table.writeInfo(os, cells[x][y]);
			if (info.needWrite)
				cells[x][y]->writeObject(os, table);
		}

	info = table.writeInfo(os, trashCell);
	if (info.needWrite)
		trashCell->writeObject(os, table);

	Serializer::writeControlPoint(os);

	info = table.writeInfo(os, &controller);
	if (info.needWrite) 
		controller.writeObject(os, table);

	int listenersCount = _listeners.getSize();
	Serializer::writeInt(os, &listenersCount);
	for (BoardListener* listener : _listeners) {
		info = table.writeInfo(os, listener);
		assert(!info.needWrite);
	}

	Serializer::writeControlPoint(os);
}

GameBoard::GameBoard(int width, int height, int screenWidth, int screenHeight) {
	_mouseTracker.setViewport(&view.viewport());
	cells = new Cell**[width];

	for (int i = 0; i < width; i++)
		cells[i] = new Cell*[height];

	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			cells[x][y] = new Cell(*this, sf::Vector2i(x, y));

	cellsCount = sf::Vector2i(width, height);
	onResize(screenWidth, screenHeight);
	controller = ObjectsController(this);
}

GameBoard::GameBoard(const GameBoard& from) {
	view = BoardView(from.view);
	_mouseTracker.setViewport(&view.viewport());
	cellsCount = from.cellsCount;

	cells = new Cell**[cellsCount.x];

	for (int i = 0; i < cellsCount.x; i++)
		cells[i] = new Cell*[cellsCount.y];

	for (shared_ptr<GameObject> child : from.children)
		attach(child);

	controller = ObjectsController(from.controller);
	controller.board = this;
}

GameBoard::~GameBoard() {
	for (int i = 0; i < cellsCount.x; i++) {
		for (int j = 0; j < cellsCount.y; j++)
			delete cells[i][j];
		delete[] cells[i];
	}
	delete[] cells;
	delete trashCell;
}

void GameBoard::validateListeners() {
	_listeners.removeIf([](BoardListener* listener) {
		if (listener->_attachState == BoardListener::WAIT_FOR_DETACH) {
			listener->_attachState = BoardListener::DETACHED;
			return true;
		}

		return false;
	});

	for (auto& listener : _listeners) 
		if (listener->_attachState == BoardListener::WAIT_FOR_ATTACH) 
			listener->_attachState = BoardListener::ATTACHED;
}

void GameBoard::onFrameStart() {
	validateListeners();

	bool mapChanged = false;

	children.removeIf([&mapChanged](shared_ptr<GameObject>& child) {
		if (child->attachState == GameObject::WAIT_FOR_DETACH) {
			child->detach();
			child->attachState = GameObject::DETACHED;
			mapChanged = true;
			return true;
		}

		return false;
	});

	for (shared_ptr<GameObject>& child : children) {
		if (child->attachState == GameObject::WAIT_FOR_ATTACH) {
			child->attachState = GameObject::ATTACHED;
			mapChanged = true;
		}
	}

	if (mapChanged) {
		notifyMapChanged();
	}

	for (shared_ptr<GameObject>& child : children) 
		if (child->attachState == GameObject::ATTACHED)
			child->onFrameStart();
}

List<shared_ptr<GameObject>>& GameBoard::getChildren() {
	return children;
}

const BoardView& GameBoard::getView() const {
	return view;
}

bool GameBoard::onEvent(sf::Event event) {
	if (_mouseTracker.processEvent(event))
		return true;

	if (event.type == sf::Event::MouseWheelScrolled
		&& event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
		view._viewport.setScale(view.viewport().getScale() + (float)event.mouseWheelScroll.delta);
		return true;
	}

	if (event.type == sf::Event::KeyReleased) {
		for (auto child : children) {
			if (child->isAttached() && child->onKeyPressed(event))
				return true;
		}
	}

	return false;
}

void GameBoard::onFrame(long timePassed) {
	for (shared_ptr<GameObject>& child : children) {
		if (child->attachState == GameObject::ATTACHED)
			child->onFrame(timePassed);
	}

	for (shared_ptr<GameObject>& child : children) {
		if (child->attachState == GameObject::ATTACHED)
			child->onPreDraw();
	}
}

void GameBoard::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (int x = 0; x < cellsCount.x; x++)
		for (int y = 0; y < cellsCount.y; y++)
			target.draw(*cells[x][y]);

	target.draw(view.gridDrawer(), states);

	for (auto child : children)
		if (child->attachState == GameObject::ATTACHED)
			child->onDraw(target, states);
}

void GameBoard::onFrameEnd() {
	for (auto child : children)
		if (child->attachState == GameObject::ATTACHED)
			child->onFrameEnd();
}


const sf::Vector2i& GameBoard::getCellsCount() const {
	return cellsCount;
}

bool GameBoard::attach(shared_ptr<GameObject> object) {
	if (object->isAttached())
		return false;

	children.add(object);
	object->attach(this);
	addListener(object.get());
	object->attachState = GameObject::WAIT_FOR_ATTACH;
	Cell& cell = getCell(object->getOccupiedCell());
	cell.add(object.get());
	notifyObjectAttached(object);

	LogInfo info;
	info.name = "GameBoard:attach";
	info.pushParam("object", object->toStringShort());
	Log::log(info);

	return true;
}

void GameBoard::detach(shared_ptr<GameObject> object) {
	if (!object->isAttached())
		return;

	Cell& cell = getCell(object->getOccupiedCell());
	cell.remove(object.get());
	removeListener(object.get());
	object->attachState = GameObject::WAIT_FOR_DETACH;
	notifyObjectDetached(object);

	LogInfo info;
	info.name = "GameBoard:detach";
	info.pushParam("object", object->toStringShort());
	Log::log(info);
}

bool GameBoard::detach(int objectId) {
	for (auto object : children) {
		if (object->id() == objectId) {
			detach(object);
			return true;
		}
	}

	return false;
}

ObjectsController& GameBoard::getController() {
	return controller;
}

void GameBoard::onResize(int width, int height) {
	float side = fminf(((float)width) / cellsCount.x, ((float)height) / cellsCount.y);
	view.update(sf::Vector2f((float)width, (float)height), cellsCount);
}

void GameBoard::moveObject(int objectId, sf::Vector2i& from, sf::Vector2i& to) {
	for (auto& child : children) {
		if (child->id() == objectId)
			return moveObject(child, from, to);
	}
}

void GameBoard::moveObject(shared_ptr<GameObject> object, sf::Vector2i& from, sf::Vector2i& to) {
	(*cells[from.x][from.y]).remove(object.get());
	(*cells[to.x][to.y]).add(object.get());
	notifyMapChanged();
	if (object->isAttached()) {
		notifyObjectMoved(object, from, to);

		LogInfo info;
		info.name = "GameBoard:moveObject";
		info.pushParam("object", object->toStringShort());
		info.pushParam("fromX", std::to_string(from.x));
		info.pushParam("fromY", std::to_string(from.y));
		info.pushParam("toX", std::to_string(to.x));
		info.pushParam("toY", std::to_string(to.y));
		Log::log(info);
	}
}

Cell& GameBoard::getCell(const sf::Vector2i& coords) const {
	if (!hasCell(coords))
		return *trashCell;

	return *cells[coords.x][coords.y];
}

bool GameBoard::hasCell(const sf::Vector2i& cellPosition) const {
	return cellPosition.x >= 0 && cellPosition.x < cellsCount.x
		&& cellPosition.y >= 0 && cellPosition.y < cellsCount.y;
}

void GameBoard::onClickLeft(sf::Vector2f& coords, bool wasMoved) {
	if (wasMoved)
		return;

	sf::Vector2i cellPosition = view.mapCoordsToCell(coords);

	if (!hasCell(cellPosition))
		return;

	bool processed = false;

	for (shared_ptr<GameObject> child : children) {
		if (child->isAttached() && child->isFocused())
			processed |= child->onClick(cellPosition);
	}

	if (processed)
		return;

	for (shared_ptr<GameObject>& child : children)
		if (child->isAttached() && !child->isFocused() && child->onClick(cellPosition))
			return;
}

void GameBoard::onMove(float dx, float dy) {
	if (!_mouseTracker.isLeftPressed())
		return;

	view._viewport.move(-dx, -dy);
}

void GameBoard::notifyMapChanged() {
	for (auto child : children)
		child->onMapChanged();
}

GameBoard* GameBoard::copy() {
	return new GameBoard(*this);
}

void GameBoard::addListener(BoardListener* listener) {
	if (listener->_attachState != BoardListener::DETACHED || _listeners.contains(listener)) {
		Log::exception("GameBoard", "Failed add listener: listener is not detached");
	}

	listener->_attachState = BoardListener::WAIT_FOR_ATTACH;

	if (!_listeners.contains(listener))
		_listeners.add(listener);
}

void GameBoard::removeListener(BoardListener* listener) {
	if (!_listeners.contains(listener)) {
		Log::exception("GameBoard", "Failed remove listener: listener is not attached");
	}

	listener->_attachState = BoardListener::WAIT_FOR_DETACH;
}

const MouseTracker& GameBoard::mouseTracker() const {
	return _mouseTracker;
}

void GameBoard::notifyObjectAttached(shared_ptr<GameObject> object) {
	for (auto listener : _listeners)
		if (listener->_attachState == BoardListener::ATTACHED)
			listener->onObjectAttached(*object);
}

void GameBoard::notifyObjectMoved(shared_ptr<GameObject> object, sf::Vector2i& from, sf::Vector2i& to) {
	for (auto listener : _listeners)
		if (listener->_attachState == BoardListener::ATTACHED)
			listener->onObjectMoved(*object, from, to);
}

void GameBoard::notifyObjectDetached(shared_ptr<GameObject> object) {
	for (auto listener : _listeners)
		if (listener->_attachState == BoardListener::ATTACHED)
			listener->onObjectDetached(*object);
}
