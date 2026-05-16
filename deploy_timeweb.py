#!/usr/bin/env python3
"""
Деплой сайта AIM на Timeweb через rsync/SSH.

Креды читаются из .env рядом со скриптом (НЕ коммитить!).
Зависимости: rsync, ssh, sshpass  (sudo dnf install sshpass)

Запуск:
    python3 deploy_timeweb.py           # обычный деплой
    python3 deploy_timeweb.py --dry-run # проверка без заливки
    python3 deploy_timeweb.py --delete  # удалять на сервере то, чего нет локально
"""
import os
import sys
import subprocess

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
ENV_FILE = os.path.join(SCRIPT_DIR, ".env")

# Что НЕ заливать на боевой сервер.
# Префикс "/" — паттерн привязан к КОРНЮ проекта (не матчит вложенные одноимённые файлы).
# Без префикса — матчится на любом уровне (использовать только для уникальных имён).
IGNORE_LIST = [
    # git/служебное
    ".git", ".gitignore", ".gitattributes", ".editorconfig",
    # GitHub Actions (CI/CD, на проде не нужно)
    "/.github",
    # claude/IDE — локальные данные агента, могут содержать секреты/transcripts
    "CLAUDE.md", ".claude", ".idea", ".vscode",
    # деплой-инфра (на сервер не выкладываем)
    "/.env", "/.env.example", "/deploy_timeweb.py", "/scripts",
    # node/сборка
    "node_modules", "/package.json", "/package-lock.json",
    "/webpack.common.js", "/webpack.config.dev.js", "/webpack.config.prod.js",
    "dist", ".cache",
    # архивы и распакованные релизы
    "*.zip", "*.rar", "*.7z", "*.tar.gz",
    "AIM Облако", "Release AIM v0.1 29-03-2026",
    # презентации, лицензия (на сайте не нужны)
    "*.pptx",
    "/LICENSE.txt",
    # кастомная 404 — НЕ исключаем, нужна для .htaccess
]


def load_env(path: str) -> dict:
    if not os.path.exists(path):
        sys.exit(f"❌ Не найден {path}\n   Скопируй .env.example → .env и заполни.")
    env = {}
    with open(path, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#") or "=" not in line:
                continue
            k, v = line.split("=", 1)
            env[k.strip()] = v.strip().strip('"').strip("'")
    return env


def main():
    env = load_env(ENV_FILE)
    required = ("TIMEWEB_HOST", "TIMEWEB_USER", "TIMEWEB_PASS", "TIMEWEB_REMOTE_DIR")
    missing = [k for k in required if not env.get(k)]
    if missing:
        sys.exit(f"❌ В .env не заполнены: {', '.join(missing)}")

    host = env["TIMEWEB_HOST"]
    user = env["TIMEWEB_USER"]
    password = env["TIMEWEB_PASS"]
    remote_dir = env["TIMEWEB_REMOTE_DIR"]

    extra_args = sys.argv[1:]
    dry_run = "--dry-run" in extra_args
    use_delete = "--delete" in extra_args

    excludes = []
    for item in IGNORE_LIST:
        excludes.extend(["--exclude", item])

    rsync_flags = ["-avz", "--progress"]
    if dry_run:
        rsync_flags.append("--dry-run")
    if use_delete:
        rsync_flags.append("--delete")

    cmd = [
        "sshpass", "-p", password,
        "rsync", *rsync_flags,
        *excludes,
        "-e", "ssh -o StrictHostKeyChecking=no",
        f"{SCRIPT_DIR}/",
        f"{user}@{host}:{remote_dir}",
    ]

    print("=" * 50)
    print("  Деплой AIM на Timeweb (rsync/SSH)")
    print("=" * 50)
    print(f"Источник: {SCRIPT_DIR}")
    print(f"Сервер:   {user}@{host}:{remote_dir}")
    print(f"Режим:    {'DRY-RUN (без заливки)' if dry_run else 'РЕАЛЬНЫЙ ДЕПЛОЙ'}"
          f"{' + --delete' if use_delete else ''}")
    print(f"Исключений: {len(IGNORE_LIST)}")
    print()

    result = subprocess.run(cmd)
    if result.returncode == 0:
        print("\n✅ Готово.")
    else:
        sys.exit(f"\n❌ Ошибка деплоя (код {result.returncode})")


if __name__ == "__main__":
    main()
