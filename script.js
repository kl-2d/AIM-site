// Mobile nav toggle
const burger = document.querySelector('.nav__burger');
const nav = document.querySelector('.nav');

if (burger && nav) {
  burger.addEventListener('click', () => {
    const open = nav.classList.toggle('nav--open');
    burger.setAttribute('aria-expanded', String(open));
  });

  document.querySelectorAll('.nav__links a').forEach(link => {
    link.addEventListener('click', () => {
      nav.classList.remove('nav--open');
      burger.setAttribute('aria-expanded', 'false');
    });
  });

  document.addEventListener('click', e => {
    if (!nav.contains(e.target)) {
      nav.classList.remove('nav--open');
      burger.setAttribute('aria-expanded', 'false');
    }
  });
}

// Video lightbox — opens any element with [data-video] in a <dialog>
(() => {
  const modal = document.getElementById('videoModal');
  if (!modal) return;
  const modalVideo = modal.querySelector('.video-modal__video');
  const modalTitle = modal.querySelector('.video-modal__title');
  const closeBtn = modal.querySelector('.js-video-close');

  function open(src, title) {
    modalVideo.src = src;
    modalTitle.textContent = title || '';
    if (typeof modal.showModal === 'function') {
      modal.showModal();
    } else {
      modal.setAttribute('open', '');
    }
    modalVideo.play().catch(() => {});
  }

  function close() {
    modalVideo.pause();
    modalVideo.removeAttribute('src');
    modalVideo.load();
    if (typeof modal.close === 'function') modal.close();
    else modal.removeAttribute('open');
  }

  document.querySelectorAll('.js-video-open').forEach(el => {
    el.addEventListener('click', () => {
      const src = el.dataset.video;
      const title = el.dataset.title;
      if (src) open(src, title);
    });
  });

  closeBtn.addEventListener('click', close);

  // click on backdrop closes
  modal.addEventListener('click', e => {
    if (e.target === modal) close();
  });

  // Esc handled natively by <dialog>, but ensure cleanup on close event
  modal.addEventListener('close', () => {
    modalVideo.pause();
  });
})();
