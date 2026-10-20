/* Фронтенд без сборки: чистый ES-модуль на API бэкенда.
 * Внутри контейнера всё раздаётся uvicorn, отдельный nginx не нужен.
 */

const API = "/api";

const state = {
  view: "items",
  items:    { page: 1, q: "", type_id: "", min_level: "", max_level: "", sort: "id" },
  monsters: { page: 1, q: "", region: "", min_level: "", max_level: "" },
  npcs:     { page: 1, q: "", region: "", shop_only: false },
};

const $  = (sel) => document.querySelector(sel);
const $$ = (sel) => Array.from(document.querySelectorAll(sel));

const nf = new Intl.NumberFormat("ru-RU");
const fmt = (n) => (n === null || n === undefined ? "—" : nf.format(n));

function toast(text) {
  const el = $("#toast");
  el.textContent = text;
  el.classList.add("show");
  clearTimeout(toast._t);
  toast._t = setTimeout(() => el.classList.remove("show"), 1600);
}

async function api(path, params = {}) {
  const url = new URL(API + path, window.location.origin);
  for (const [k, v] of Object.entries(params)) {
    if (v !== "" && v !== null && v !== undefined) url.searchParams.set(k, v);
  }
  const res = await fetch(url);
  if (!res.ok) {
    const detail = await res.json().catch(() => ({}));
    throw new Error(detail.detail || `HTTP ${res.status}`);
  }
  return res.json();
}

/* --------------------------------------------------------------- вкладки */
$$(".tab").forEach((btn) => {
  btn.addEventListener("click", () => {
    state.view = btn.dataset.view;
    showView(state.view);
    load();
  });
});

/* Кнопка «Назад» у каждой карточки знает свой список: одна и та же
   секция используется и из «Монстров», и из «Существ». */
$$("[data-back]").forEach((btn) => {
  btn.addEventListener("click", () => {
    const target = btn.dataset.back || "items";
    state.view = target;
    showView(target);
    load();
  });
});

// клики по ссылкам внутри карточек и по строкам таблиц
document.addEventListener("click", (e) => {
  const npcLink = e.target.closest("[data-npc]");
  if (npcLink) {
    e.preventDefault();
    openShop(npcLink.dataset.npc, 1, "items");
    return;
  }
  const creatureChip = e.target.closest("[data-creature]");
  if (creatureChip) {
    openCreature(creatureChip.dataset.creature, "creatures");
    return;
  }
  const monsterRow = e.target.closest("tr[data-monster]");
  if (monsterRow) {
    openMonster(monsterRow.dataset.monster);
    return;
  }
  const npcRow = e.target.closest("tr[data-npcrow]");
  if (npcRow) {
    openShop(npcRow.dataset.npcrow, 1, "npcs");
    return;
  }
  const itemRow = e.target.closest("tr[data-item]");
  if (itemRow) {
    openItem(itemRow.dataset.item, "shop");
  }
});

$$(".reset").forEach((btn) => {
  btn.addEventListener("click", () => {
    const view = btn.dataset.reset;
    state[view].page = 1;
    document.querySelectorAll(`#view-${view} .filters input`).forEach((i) => {
      if (i.type === "checkbox") i.checked = false;
      else i.value = "";
    });
    document.querySelectorAll(`#view-${view} .filters select`).forEach((s) => (s.value = ""));
    load();
  });
});

/* ------------------------------------------------------------ пагинация */
function renderPager(container, page, onGo) {
  container.innerHTML = "";
  if (page.pages <= 1) return;

  const prev = document.createElement("button");
  prev.textContent = "‹ Назад";
  prev.disabled = page.page <= 1;
  prev.onclick = () => onGo(page.page - 1);

  const info = document.createElement("span");
  info.className = "info";
  info.textContent = `Страница ${page.page} из ${page.pages} (всего ${fmt(page.total)})`;

  const next = document.createElement("button");
  next.textContent = "Вперёд ›";
  next.disabled = page.page >= page.pages;
  next.onclick = () => onGo(page.page + 1);

  container.append(prev, info, next);
}

/* ------------------------------------------------------------- предметы */
function loadItems() {
  const s = state.items;
  return api("/catalog/items", {
    q: s.q, type_id: s.type_id,
    min_level: s.min_level, max_level: s.max_level,
    sort: s.sort, page: s.page, page_size: 50,
  }).then((data) => {
    const body = $("#items-body");
    body.innerHTML = "";
    for (const it of data.items) {
      const tr = document.createElement("tr");
      tr.className = "clickable";
      tr.dataset.item = it.item_id;
      tr.innerHTML = `
        <td>${iconHtml(it.icon)}</td>
        <td>${it.item_id}</td>
        <td>${it.name}</td>
        <td class="dim">${it.type_name || "—"}</td>
        <td class="num">${fmt(it.required_level)}</td>
        <td class="num">${fmt(it.price)}</td>
        <td><code class="gm" data-cmd="make ${it.item_id},1">make ${it.item_id},1</code></td>`;
      body.append(tr);
    }
    if (!data.items.length) {
      body.innerHTML = '<tr><td colspan="6" class="dim">Ничего не найдено</td></tr>';
    }
    renderPager($("#items-pager"), data.page, (p) => { s.page = p; load(); });
  });
}

/* ------------------------------------------------------------- монстры */
function loadMonsters() {
  const s = state.monsters;
  return api("/catalog/monsters", {
    q: s.q, region: s.region,
    min_level: s.min_level, max_level: s.max_level,
    page: s.page, page_size: 50,
  }).then((data) => {
    const body = $("#mon-body");
    body.innerHTML = "";
    for (const m of data.monsters) {
      const tr = document.createElement("tr");
      tr.className = "clickable";
      tr.dataset.monster = m.monster_id;
      tr.innerHTML = `
        <td>${m.monster_id}</td>
        <td>${m.name}</td>
        <td class="num">${fmt(m.level)}</td>
        <td class="dim">${m.region || "—"}</td>
        <td class="num dim">${fmt(m.x)}</td>
        <td class="num dim">${fmt(m.y)}</td>`;
      body.append(tr);
    }
    if (!data.monsters.length) {
      body.innerHTML = '<tr><td colspan="6" class="dim">Ничего не найдено</td></tr>';
    }
    renderPager($("#mon-pager"), data.page, (p) => { s.page = p; load(); });
  });
}

/* ----------------------------------------------------------------- NPC */
function loadNpcs() {
  const s = state.npcs;
  return api("/catalog/npcs", {
    q: s.q, region: s.region, shop_only: s.shop_only, page: s.page, page_size: 50,
  })
    .then((data) => {
      const body = $("#npc-body");
      body.innerHTML = "";
      for (const n of data.npcs) {
        const tr = document.createElement("tr");
        tr.className = "clickable";
        tr.dataset.npcrow = n.npc_id;
        tr.innerHTML = `
          <td>${n.npc_id}</td>
          <td>${n.name}</td>
          <td class="dim">${n.location || "—"}</td>
          <td class="dim">${n.region || "—"}</td>
          <td class="num dim">${fmt(n.x)}</td>
          <td class="num dim">${fmt(n.y)}</td>`;
        body.append(tr);
      }
      if (!data.npcs.length) {
        body.innerHTML = '<tr><td colspan="6" class="dim">Ничего не найдено</td></tr>';
      }
      renderPager($("#npc-pager"), data.page, (p) => { s.page = p; load(); });
    });
}

/* --------------------------------------------------------------- опыт */
function loadLevels() {
  const min = $("#lvl-min").value || 1;
  const max = $("#lvl-max").value || 150;
  return api("/levels/costs", { min_level: min, max_level: max }).then((rows) => {
    const body = $("#lvl-body");
    body.innerHTML = "";
    let prev = null;
    for (const r of rows) {
      const pct = prev ? ((r.exp_cost - prev) / prev * 100) : 0;
      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td class="num">${r.level}</td>
        <td class="num">${fmt(r.exp_total)}</td>
        <td class="num">${fmt(r.exp_cost)}</td>
        <td class="num dim">${prev ? pct.toFixed(2) + " %" : "—"}</td>`;
      body.append(tr);
      prev = r.exp_cost;
    }
  });
}

/* -------------------------------------------------------- калькулятор */
const STAT_LABELS = {
  max_hp: "Максимум HP", max_sp: "Максимум SP",
  min_attack: "Мин. атака", max_attack: "Макс. атака",
  defence: "Защита", resist: "Сопротивление",
  hit: "Точность", flee: "Уклонение", magic: "Магия",
  crit: "Крит", hp_recover: "Реген. HP", sp_recover: "Реген. SP",
  attack_speed: "Скор. атаки",
  ship_defence: "Защита корабля", ship_attack: "Атака корабля",
};

async function runCalculator() {
  const payload = {
    job: +$("#calc-job").value,
    level: +$("#calc-level").value,
    strength: +$("#calc-str").value,
    dexterity: +$("#calc-dex").value,
    agility: +$("#calc-agi").value,
    constitution: +$("#calc-con").value,
    stamina: +$("#calc-sta").value,
    luck: +$("#calc-luk").value,
    ship_level: +$("#calc-ship").value,
  };
  const data = await fetch(API + "/calculator/stats", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(payload),
  }).then((r) => r.json());

  const box = $("#calc-result");
  box.innerHTML = "";
  for (const [key, label] of Object.entries(STAT_LABELS)) {
    if (data[key] === undefined) continue;
    const div = document.createElement("div");
    div.className = "stat";
    div.innerHTML = `<span class="k">${label}</span><span class="v">${fmt(data[key])}</span>`;
    box.append(div);
  }
}

/* --------------------------------------------------------------- карточки */
function showView(name) {
  $$(".tab").forEach((b) => b.classList.toggle("active", b.dataset.view === name));
  $$(".view").forEach((v) => v.classList.add("hidden"));
  const target = $("#view-" + name);
  if (target) target.classList.remove("hidden");
  window.scrollTo(0, 0);
}

function iconHtml(icon) {
  if (!icon) return "";
  return `<img class="ic" src="/assets/icon/${icon}.png" alt="" loading="lazy"
               onerror="this.style.visibility='hidden'">`;
}

// Поля, у которых коэффициент показывается в игре процентом.
const PCT_BONUS = new Set(["ASPD", "ADIS", "MSPD", "COL", "HREC", "SREC", "MF", "CRT"]);

// Коэффициенты хранятся в десятых: 50 -> +5%, 340 -> +34%.
const tenths = (n) => (n / 10).toFixed(1).replace(/\.0$/, "");

function fmtBonus(b) {
  const unit = (v) => (PCT_BONUS.has(b.field) ? `${tenths(v)}%` : tenths(v));
  const flat = b.min || b.max;
  const coef = b.coef_min || b.coef_max;
  const sign = (v) => (v > 0 ? `+${unit(v)}` : unit(v));

  if (flat && coef) return `${sign(b.min)}–${sign(b.max)} и ${unit(b.coef_min)}–${unit(b.coef_max)} за ед.`;
  if (coef) return `${sign(b.coef_min)}–${sign(b.coef_max)} за ед.`;
  return `${sign(b.min)}–${sign(b.max)}`;
}

function flagRow(flags) {
  const names = {
    trade: "Торгуется", pickup: "Подбирается", throw: "Выбрасывается",
    destroy: "Уничтожается", instantiated: "Инстансный", exclusive: "Эксклюзивный",
  };
  return Object.entries(flags).map(([k, v]) =>
    `<span class="flag ${v ? "yes" : "no"}">${names[k] || k}</span>`).join("");
}

const BACK_TEXT = {
  items: "← Назад к предметам",
  monsters: "← Назад к монстрам",
  creatures: "← Назад к существам",
  npcs: "← Назад к NPC",
  shop: "← Назад к торговцу",
};

async function openItem(itemId, back = "items") {
  showView("item");
  $("#item-back").dataset.back = back;
  $("#item-back").textContent = BACK_TEXT[back] || "← Назад";
  const box = $("#item-card");
  box.innerHTML = '<div class="loading">Загрузка…</div>';
  const d = await api(`/catalog/items/${itemId}/full`);

  const bonuses = d.bonuses.length
    ? `<table class="kv"><tbody>${d.bonuses.map((b) =>
        `<tr><td>${b.title}</td><td class="num">${fmtBonus(b)}</td></tr>`).join("")}</tbody></table>`
    : '<p class="dim">Нет бонусов</p>';

  const shops = d.sold_by.length
    ? `<table class="kv"><tbody>${d.sold_by.map((s) =>
        `<tr><td><a href="#" data-npc="${s.npc_id}">${s.npc_name}</a></td>
             <td class="dim">${s.region || ""}</td>
             <td class="num dim">${s.x}, ${s.y}</td></tr>`).join("")}</tbody></table>`
    : '<p class="dim">Нигде не продаётся (добывается из существ, квестов, создаётся)</p>';

  box.innerHTML = `
    <div class="cardhead">
      ${iconHtml(d.icon)}
      <div>
        <h1>${d.name}</h1>
        <div class="sub">${d.type_name || "—"} · ID <b>${d.item_id}</b></div>
        <div class="gm"><code class="gm" data-cmd="${d.gm_command}">${d.gm_command}</code></div>
      </div>
    </div>
    ${d.description ? `<p class="desc">${d.description}</p>` : ""}
    <div class="props">${flagRow(d.flags)}</div>
    <div class="cardgrid">
      <section><h2>Основное</h2>
        <table class="kv"><tbody>
          <tr><td>Цена</td><td class="num">${fmt(d.price)}</td></tr>
          <tr><td>Требуемый уровень</td><td class="num">${fmt(d.required_level)}</td></tr>
          <tr><td>Размер стака</td><td class="num">${fmt(d.pile_max)}</td></tr>
          <tr><td>Прочность</td><td class="num">${fmt(d.endurance)}</td></tr>
          <tr><td>Отверстий</td><td class="num">${fmt(d.holes)}</td></tr>
          <tr><td>Уровень ковки</td><td class="num">${fmt(d.forge_level)}</td></tr>
        </tbody></table>
      </section>
      <section><h2>Требования</h2>
        <p><b>Профессии:</b> ${d.jobs.join(", ") || "любая"}</p>
        <p><b>Слоты:</b> ${d.equip_slots.join(", ") || "не экипируется"}</p>
        ${d.use_effect ? `<p><b>Эффект:</b> ${d.use_effect}</p>` : ""}
      </section>
      <section><h2>Бонусы</h2>${bonuses}</section>
      <section><h2>Где продаётся</h2>${shops}</section>
    </div>`;
}

/* Компактная карточка существа.

Монстр не стоит на месте, поэтому показываем не отдельные точки,
а области: рамку вокруг всех точек в регионе, их количество и респаун.
То же, что на piratia.arbah.ru. */
function renderCreatureCard(box, d) {
  const respawn = d.respawns.length
    ? [...new Set(d.respawns)].sort((a, b) => a - b)
        .map((r) => r >= 60 ? `${Math.floor(r / 60)} мин` : `${r} с`).join(", ")
    : "не указан";

  const areas = d.areas.length
    ? `<div class="spawnwrap"><table class="spawns">
         <thead><tr><th class="num">#</th><th>Область</th>
           <th class="num">Кол-во</th><th class="num">Восстановление</th>
           <th>Границы</th></tr></thead>
         <tbody>${d.areas.map((a, i) => `
           <tr><td class="num dim">${i + 1}</td>
               <td>${a.region}</td>
               <td class="num">${fmt(a.count)}</td>
               <td class="num">${[...new Set(a.respawns)]
                 .map((r) => r >= 60 ? `${Math.floor(r / 60)} мин` : `${r} с`)
                 .join(", ")}</td>
               <td class="num dim">${a.area}</td></tr>`).join("")}</tbody>
       </table></div>
       <p class="dim">Монстр бродит внутри этих границ, точные координаты спавна
          показаны ниже списком.</p>`
    : '<p class="dim">Статических точек спавна нет — существо создаётся по событию (морские встречи, инстансы).</p>';

  const points = d.spawns.length
    ? `<div class="pointswrap">
         <button class="btn toggle" id="pts-toggle" aria-expanded="false">
           Показать все точки спавна (${fmt(d.spawn_count)})
         </button>
         <div class="spawnwrap hidden" id="pts-box">
           <table class="spawns">
             <thead><tr><th class="num">#</th><th>Область</th>
               <th class="num">X</th><th class="num">Y</th></tr></thead>
             <tbody>${d.spawns.map((s, i) => `
               <tr><td class="num dim">${i + 1}</td><td>${s.region || "—"}</td>
                   <td class="num">${s.x}</td><td class="num">${s.y}</td></tr>`).join("")}
             </tbody></table>
           ${d.spawns_truncated
             ? `<p class="dim">Показаны первые 400 из ${fmt(d.spawn_count)}.</p>` : ""}
         </div>
       </div>`
    : "";

  // Боевые характеристики приходят с русскими подписями из БД:
  // ключ "level" -> значение в "level", подпись в "level_title".
  const STAT_ORDER = [
    "level", "max_hp", "max_sp", "min_atk", "max_atk", "phys_res",
    "defense", "hit", "dodge", "atk_speed", "move_speed", "exp",
  ];

  const stats = d.stats
    ? `<h2 class="sect">Боевые характеристики</h2>
       <table class="grid">
         ${STAT_ORDER.map((k, i) => {
           const cell = `<th>${d.stats[k + "_title"]}</th>` +
                        `<td>${fmt(d.stats[k])}</td>`;
           return i % 2 === 0 ? `<tr>${cell}` : `${cell}</tr>`;
         }).join("")}
       </table>
       <p class="dim">Значения взяты из таблицы существ без учёта рейтов
          сервера — это базовые характеристики.</p>`
    : "";

  const drops = d.drops && d.drops.length
    ? `<h2 class="sect">Дроп (${d.drops.length})</h2>
       <div class="tablewrap">
         <table class="spawns">
           <thead><tr><th class="num">#</th><th>Предмет</th>
             <th class="num">ID</th><th class="num">Частота</th>
             <th class="num">Ур.</th></tr></thead>
           <tbody>${d.drops.map((x, i) => `
             <tr><td class="num dim">${i + 1}</td>
                 <td>${iconHtml(x.icon)}<span>${x.name || `предмет ${x.item_id}`}</span></td>
                 <td class="num dim">${x.item_id}</td>
                 <td class="num">${fmt(x.rate)}</td>
                 <td class="num">${x.required_level || "—"}</td></tr>`).join("")}
           </tbody>
         </table>
       </div>
       <p class="dim">Частота указана так, как хранится в игре: 10000
          соответствует 100 %, а отображаемый в клиенте процент
          дополнительно умножается на рейт дропа сервера.</p>`
    : "";

  box.innerHTML = `
    <div class="cchead">
      ${d.image_url ? `<img class="portrait" src="${d.image_url}.bmp" alt=""
            onerror="this.style.display='none'">`
        : '<div class="portrait placeholder"></div>'}
      <div class="ccheadtext">
        <h1>${d.name}</h1>
        ${d.short_name && d.short_name !== d.name
          ? `<div class="sub">${d.short_name}</div>` : ""}
      </div>
    </div>

    <table class="grid">
      <tr><th>Уровень</th><td>${d.level ?? "—"}</td>
          <th>Всего точек</th><td>${fmt(d.spawn_count)}</td></tr>
      <tr><th>Восстановление</th><td>${respawn}</td>
          <th>ID существа</th><td>${d.creature_id ?? "—"}</td></tr>
    </table>

    ${stats}
    ${drops}

    <h2 class="sect">Где обитает</h2>
    ${areas}
    ${points}`;

  // Ищем внутри карточки: хелпер $ ищет по всему документу
  // и молча игнорирует второй аргумент.
  const toggle = box.querySelector("#pts-toggle");
  const ptsBox = box.querySelector("#pts-box");
  if (toggle && ptsBox) {
    toggle.addEventListener("click", () => {
      const nowHidden = ptsBox.classList.toggle("hidden");
      toggle.textContent = nowHidden
        ? `Показать все точки спавна (${fmt(d.spawn_count)})`
        : "Свернуть список точек";
      toggle.setAttribute("aria-expanded", String(!nowHidden));
    });
  }
}

async function openCreature(creatureId, back = "creatures") {
  showView("creature");
  $("#creature-back").dataset.back = back;
  $("#creature-back").textContent = BACK_TEXT[back] || "← Назад";
  const box = $("#creature-card");
  box.innerHTML = '<div class="loading">Загрузка…</div>';
  try {
    const d = await api(`/catalog/creatures/${creatureId}/full`);
    renderCreatureCard(box, d);
  } catch (e) {
    box.innerHTML = `<p class="dim">Ошибка: ${e.message}</p>`;
  }
}

async function openMonster(monsterId) {
  showView("creature");
  $("#creature-back").dataset.back = "monsters";
  $("#creature-back").textContent = BACK_TEXT.monsters;
  const box = $("#creature-card");
  box.innerHTML = '<div class="loading">Загрузка…</div>';
  try {
    const d = await api(`/catalog/monsters/${monsterId}/full`);
    renderCreatureCard(box, d);
    if (!d.creature_id) {
      box.insertAdjacentHTML("beforeend",
        '<p class="dim">Этой записи нет в реестре существ, точки спавна не показаны.</p>');
    }
  } catch (e) {
    box.innerHTML = `<p class="dim">Ошибка: ${e.message}</p>`;
  }
}

async function openShop(npcId, page = 1, back = "npcs") {
  showView("shop");
  $("#shop-back").dataset.back = back;
  $("#shop-back").textContent = BACK_TEXT[back] || "← Назад";
  const box = $("#shop-card");
  box.innerHTML = '<div class="loading">Загрузка…</div>';
  const d = await api(`/catalog/npcs/${npcId}/shop`, { page, page_size: 60 });

  const rows = d.items.map((it, i) => `
    <tr data-item="${it.item_id}" class="clickable">
      <td class="num dim">${page > 1 ? (page - 1) * 60 + i + 1 : i + 1}</td>
      <td class="iccell">${iconHtml(it.icon)}</td>
      <td class="dim">${it.type_name || "—"}</td>
      <td>${it.name}</td>
      <td class="num">${fmt(it.price)}</td>
      <td><code class="gm" data-cmd="${it.gm_command}">${it.gm_command}</code></td>
    </tr>`).join("");

  const sells = d.page.total > 0
    ? `<h2 class="sect">Продаёт · ${fmt(d.page.total)}</h2>
       <div class="tablewrap"><table>
         <thead><tr><th class="num">#</th><th></th><th>Тип</th><th>Название</th>
           <th class="num">Цена</th><th>GM-команда</th></tr></thead>
         <tbody>${rows}</tbody></table></div>
       <div class="pager" id="shop-pager"></div>`
    : `<h2 class="sect">Ассортимент</h2>
       <p class="dim">Этот NPC ничего не продаёт: он квестовый, ремесленный
          или служебный. Торговые точки выделены отдельным фильтром
          в списке NPC.</p>`;

  box.innerHTML = `
    <div class="cchead">
      ${d.npc.image_url ? `<img class="portrait" src="${d.npc.image_url}.bmp" alt=""
            onerror="this.style.display='none'">` : '<div class="portrait placeholder"></div>'}
      <div class="ccheadtext">
        <h1>${d.npc.name}</h1>
        <div class="sub">ID ${d.npc.npc_id}</div>
      </div>
    </div>

    <table class="grid">
      <tr><th>Карта</th><td class="wrap">${d.npc.region || "—"}</td>
          <th>Координаты</th><td>${d.npc.x ?? "—"} / ${d.npc.y ?? "—"}</td></tr>
      ${d.npc.location ? `<tr><th>Место</th><td class="wrap" colspan="3">${d.npc.location}</td></tr>` : ""}
    </table>

    ${d.npc.dialogue ? `<p class="dialogue">«${d.npc.dialogue}»</p>` : ""}
    ${sells}`;

  const pager = box.querySelector("#shop-pager");
  if (pager) {
    renderPager(pager, d.page, (p) => openShop(npcId, p, back));
  }
}

async function searchCreatures() {
  const q = $("#cr-q").value.trim();
  const spawned = $("#cr-spawned").checked;
  const box = $("#cr-list");
  if (!q) { box.innerHTML = '<p class="dim">Начните вводить название</p>'; return; }
  try {
    const list = await api("/catalog/creatures/search",
      { q, spawned_only: spawned, limit: 60 });
    box.innerHTML = list.length ? list.map((c) => `
      <button class="chip" data-creature="${c.creature_id}">
        <b>${c.name}</b>${c.short_name && c.short_name !== c.name
          ? `<span class="dim"> ${c.short_name}</span>` : ""}
        <span class="dim">ID ${c.creature_id} · ${fmt(c.spawn_count)} точек</span>
      </button>`).join("")
      : '<p class="dim">Ничего не найдено</p>';
  } catch (e) {
    box.innerHTML = `<p class="dim">Ошибка: ${e.message}</p>`;
  }
}

/* -------------------------------------------------------------- данжи */
// Часы приходят с сервера готовыми строками: клиент не переводит epoch
// сам, поэтому часовой пояс машины на отображение не влияет.

const fmtLeft = (sec) => {
  const s = Math.max(0, Math.floor(sec));
  const h = Math.floor(s / 3600);
  const m = Math.floor((s % 3600) / 60);
  return h ? `${h} ч ${String(m).padStart(2, "0")} мин`
           : `${m} мин ${String(s % 60).padStart(2, "0")} с`;
};

// Стенные часы: сервер отдал HH:MM, дальше прибавляем секунды,
// прошедшие с момента ответа. Разница в миллисекундах делится на 1000.
let serverClockBase = "00:00";
let serverClockBaseMs = Date.now();
const serverClock = () => {
  const [h, m] = serverClockBase.split(":").map(Number);
  const sec = Math.floor((Date.now() - serverClockBaseMs) / 1000);
  const s = (h * 3600 + m * 60 + sec + 86400) % 86400;
  return `${String(Math.floor(s / 3600)).padStart(2, "0")}:${String(
    Math.floor((s % 3600) / 60)).padStart(2, "0")}`;
};

// Состояние живёт между перерисовками: ответ сервера запрашивается один раз,
// таймер тикает на клиенте.
let dungeonState = null;

function renderDungeons() {
  if (!dungeonState) return;
  const box = $("#dungeons-body");
  const now = dungeonState.now;

  const phaseOf = (ev) => {
    if (now < ev.open_at) return "wait";
    if (now < ev.entry_closed_at) return "open";
    if (now < ev.closed_at) return "inside";
    return "done";
  };

  const describe = (d) => {
    const ev = d.events.find((e) => phaseOf(e) !== "done");
    if (!ev) return ["—", ""];
    switch (phaseOf(ev)) {
      case "wait":
        return ["открытие", `через ${fmtLeft(ev.open_at - now)}`];
      case "open":
        return ["открытие", `вход ещё ${fmtLeft(ev.entry_closed_at - now)}`];
      case "inside":
        return ["закрытие", `карта ещё ${fmtLeft(ev.closed_at - now)}`];
      default:
        return ["—", ""];
    }
  };

  const rows = dungeonState.dungeons.filter((x) => !x.always_open);
  const free = dungeonState.dungeons.filter((x) => x.always_open);

  const render = (d) => {
    const [phase, left] = d.always_open ? ["свободный вход", ""] : describe(d);
    const ev = d.events[0];
    return `<tr class="${phase === "открытие" ? "open" : ""}">
        <td>${d.name}</td>
        <td class="dim">${d.map_file}</td>
        <td>${d.first_open_hm || "—"}</td>
        <td class="dim">${(ev && ev.open_hm) || "—"}</td>
        <td class="dim">${(ev && ev.entry_closed_hm) || "—"}</td>
        <td class="dim">${(ev && ev.closed_hm) || "—"}</td>
        <td>${phase}${left ? ` · ${left}` : ""}</td>
      </tr>`;
  };

  box.innerHTML =
    rows.map(render).join("") +
    (free.length
      ? `<tr class="grp"><td colspan="7">Свободный вход (${free.length})</td></tr>`
        + free.map(render).join("")
      : "");
}

async function loadDungeons() {
  const box = $("#dungeons-body");
  box.innerHTML = '<tr><td colspan="7"><div class="loading">Загрузка…</div></td></tr>';

  const d = await api("/catalog/dungeons");
  // выравниваем клиентские часы по серверным, чтобы таймер не уезжал
  // Разница между стенными часами сервера и epoch по модулю суток.
  // Всё дальше показываем в часовом поясе сервера, а не в UTC.
  serverClockBase = d.server_clock || "00:00";
  serverClockBaseMs = Date.now();

  dungeonState = {
    dungeons: d.dungeons,
    base: d.server_time,
    baseMs: Date.now(),   // Date.now() в миллисекундах
    now: d.server_time,
  };

  $("#dungeons-hour").textContent =
    `сейчас ${serverClock()} · ` +
    `${d.scheduled} из ${d.total} карт с расписанием`;

  renderDungeons();
  clearInterval(window.__dungeonTimer);
  window.__dungeonTimer = setInterval(() => {
    if (!dungeonState) return;
    dungeonState.now = dungeonState.base
      + Math.floor((Date.now() - dungeonState.baseMs) / 1000);
    $("#dungeons-hour").textContent =
      `сейчас ${serverClock()} · ` +
      `${dungeonState.dungeons.filter((x) => !x.always_open).length} из ` +
      `${dungeonState.dungeons.length} карт с расписанием`;
    renderDungeons();
  }, 1000);
}

function load() {
  const loaders = {
    items: loadItems, monsters: loadMonsters, npcs: loadNpcs,
    levels: loadLevels, dungeons: loadDungeons,
    creatures: null, calculator: null,
    item: null, creature: null, shop: null,
  };
  const fn = loaders[state.view];
  if (fn) fn().catch((e) => toast("Ошибка: " + e.message));
}

function fillSelect(sel, items, valueKey, labelFn) {
  const el = $(sel);
  items.forEach((it) => {
    const opt = document.createElement("option");
    opt.value = it[valueKey];
    opt.textContent = labelFn(it);
    el.append(opt);
  });
}

async function init() {
  // поля поиска с задержкой, чтобы не слать запрос на каждую букву
  const bind = (selector, view, field) => {
    $(selector).addEventListener("input", () => {
      clearTimeout($(selector)._t);
      $(selector)._t = setTimeout(() => {
        state[view][field] = $(selector).value;
        state[view].page = 1;
        load();
      }, 280);
    });
  };
  bind("#items-q", "items", "q");
  bind("#items-type", "items", "type_id");
  bind("#items-minlvl", "items", "min_level");
  bind("#items-maxlvl", "items", "max_level");
  bind("#mon-q", "monsters", "q");
  bind("#mon-region", "monsters", "region");
  bind("#mon-minlvl", "monsters", "min_level");
  bind("#mon-maxlvl", "monsters", "max_level");
  bind("#npc-q", "npcs", "q");
  bind("#npc-region", "npcs", "region");
  $("#npc-shop").addEventListener("change", (e) => {
    state.npcs.shop_only = e.target.checked;
    state.npcs.page = 1;
    load();
  });
  $("#cr-q").addEventListener("input", () => {
    clearTimeout($("#cr-q")._t);
    $("#cr-q")._t = setTimeout(searchCreatures, 300);
  });
  $("#cr-spawned").addEventListener("change", searchCreatures);

  const [types, regions, npcRegions, jobs, health] = await Promise.all([
    api("/catalog/items/types"),
    api("/catalog/monsters/regions"),
    api("/catalog/npcs/regions"),
    api("/calculator/jobs"),
    api("/health").catch(() => null),
  ]);

  fillSelect("#items-type", types.filter((t) => t.id !== null), "id",
    (t) => `${t.name || "Без типа"} (${t.count})`);
  fillSelect("#mon-region", regions, "name", (r) => `${r.name} (${r.count})`);
  fillSelect("#npc-region", npcRegions, "name", (r) => `${r.name} (${r.count})`);
  fillSelect("#calc-job", jobs, "code", (j) => `${j.code} — ${j.name}`);

  if (health) {
    $("#counts").innerHTML =
      `предметов: <b>${fmt(health.counts.items)}</b> · ` +
      `монстров: <b>${fmt(health.counts.monsters)}</b> · ` +
      `NPC: <b>${fmt(health.counts.npcs)}</b>`;
  }

  // копирование GM-команды по клику
  document.addEventListener("click", (e) => {
    const code = e.target.closest("code.gm");
    if (!code) return;
    navigator.clipboard.writeText(code.dataset.cmd)
      .then(() => toast("Скопировано: " + code.dataset.cmd))
      .catch(() => toast("Не удалось скопировать"));
  });

  $("#lvl-load").addEventListener("click", () => loadLevels().catch((e) => toast(e.message)));
  $("#calc-go").addEventListener("click", () => runCalculator().catch((e) => toast(e.message)));

  loadItems().catch((e) => toast("Ошибка: " + e.message));
}

document.addEventListener("DOMContentLoaded", init);