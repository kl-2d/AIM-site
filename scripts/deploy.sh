#!/usr/bin/env bash
# Запустить GitHub Actions workflow «Deploy to Timeweb» через GH API.
# Использует gh CLI. Логи деплоя пишутся прямо в этот терминал.
#
# Зачем нужен: чтобы можно было катать прод с любого устройства без SSH-доступа
# до Timeweb с локали (актуально когда у твоего провайдера DNS режется
# на *.timeweb.ru, а у GitHub-runner всё резолвится нормально).
#
# Использование:
#   scripts/deploy.sh                   # dry-run (безопасно, по умолчанию)
#   scripts/deploy.sh --real            # реальный деплой
#   scripts/deploy.sh --real --delete   # реальный + удалить чужое на сервере
#   scripts/deploy.sh --help

set -euo pipefail

DRY_RUN=true
DELETE=false
WATCH=true

print_help() {
  cat <<EOF
Триггер workflow .github/workflows/deploy.yml.

Опции:
  (без флагов)   dry-run (проверка, ничего не льётся)
  --real         реальный деплой
  --delete       (вместе с --real) удалять файлы на сервере, которых нет локально
  --no-watch     не ждать завершения run-а, только запустить
  --help         показать это сообщение

Перед первым запуском:
  1) gh auth login
  2) gh secret set TIMEWEB_HOST       -b "vh410.timeweb.ru"
     gh secret set TIMEWEB_USER       -b "cg63009"
     gh secret set TIMEWEB_PASS       -b "<пароль>"
     gh secret set TIMEWEB_REMOTE_DIR -b "AIM/public_html/"
EOF
}

for arg in "$@"; do
  case "$arg" in
    --real)     DRY_RUN=false ;;
    --delete)   DELETE=true ;;
    --no-watch) WATCH=false ;;
    --help|-h)  print_help; exit 0 ;;
    *) echo "Unknown flag: $arg" >&2; print_help; exit 2 ;;
  esac
done

if ! command -v gh >/dev/null 2>&1; then
  echo "❌ gh CLI не установлен. dnf install gh / brew install gh" >&2
  exit 1
fi

if ! gh auth status >/dev/null 2>&1; then
  echo "❌ gh не залогинен. Запусти: gh auth login" >&2
  exit 1
fi

echo "=================================================="
echo "  Triggering Timeweb deploy via GitHub Actions"
echo "=================================================="
echo "  Mode:   $([ "$DRY_RUN" = true ] && echo 'DRY-RUN' || echo 'REAL DEPLOY')"
echo "  Delete: $DELETE"
echo "  Watch:  $WATCH"
echo

gh workflow run deploy.yml \
  -f dry_run="$DRY_RUN" \
  -f delete="$DELETE"

if [ "$WATCH" = true ]; then
  echo
  echo "Жду пока run-а появится в очереди…"
  # короткая пауза, чтобы run успел зарегистрироваться
  sleep 3
  # ID последнего run по этому workflow
  RUN_ID=$(gh run list --workflow=deploy.yml --limit=1 --json databaseId --jq '.[0].databaseId')
  if [ -z "$RUN_ID" ]; then
    echo "⚠️  Не нашла свежий run, открой логи в браузере: gh run list --workflow=deploy.yml"
    exit 0
  fi
  echo "Run #$RUN_ID — стримлю логи…"
  gh run watch "$RUN_ID" --exit-status
fi
