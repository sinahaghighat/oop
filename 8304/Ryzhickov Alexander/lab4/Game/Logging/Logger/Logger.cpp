//
// Created by Alex on 14.04.2020.
//

#include "Logger.h"

void Logger::enableTerminalWriteMode() {
    proxyLogger.enableTerminalWriteMode();
}

void Logger::enableFileWriteLogMode() {
    proxyLogger.enableFileWriteLogMode();
}

void Logger::enableWrite() {
    proxyLogger.enableWrite();
}

void Logger::disableWrite() {
    proxyLogger.disableWrite();
}

void Logger::switchWriteMode() {
    proxyLogger.switchWriteMode();
}

void Logger::logAddSwordsmen(int x, int y) {
    proxyLogger.logMessage("addSwordmen  x:" + std::to_string(x) + " y:" + std::to_string(y));
}

void Logger::logAddSpearmen(int x, int y) {
    proxyLogger.logMessage("addSpearmen  x:" + std::to_string(x) + " y:" + std::to_string(y));
}

void Logger::logAddArcher(int x, int y) {
    proxyLogger.logMessage("addArcher    x:" + std::to_string(x) + " y:" + std::to_string(y));
}

void Logger::logAddMagician(int x, int y) {
    proxyLogger.logMessage("addMagician  x:" + std::to_string(x) + " y:" + std::to_string(y));
}

void Logger::logAddKing(int x, int y) {
    proxyLogger.logMessage("addKing      x:" + std::to_string(x) + " y:" + std::to_string(y));
}

void Logger::logAddKnight(int x, int y) {
    proxyLogger.logMessage("addKnight    x:" + std::to_string(x) + " y:" + std::to_string(y));
}

void Logger::logUpdatePosition(int lastX, int lastY, int newX, int newY) {
    proxyLogger.logMessage(
            "Unit go to " + std::to_string(newX) + ":" + std::to_string(newY) + " from " + std::to_string(lastX) + ":" +
            std::to_string(lastY));

}

void Logger::logUnitDie(int x, int y) {
    proxyLogger.logMessage("Cell " + std::to_string(x) + ":" + std::to_string(y) + " Unit die");
}

void Logger::logAttackUnit(int lastX, int lastY, int newX, int newY) {
    proxyLogger.logMessage(
            "Unit " + std::to_string(lastX) + ":" + std::to_string(lastY) + " attack Unit " + std::to_string(newX) + ":" +
            std::to_string(newY));
}
