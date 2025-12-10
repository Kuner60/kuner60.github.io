document.addEventListener('DOMContentLoaded', function () {
    const tabItems = document.querySelectorAll('.tab-item');
    const underline = document.querySelector('.tab-underline');
    const contents = document.querySelectorAll('.tab-content');

    // 初始化：设置第一个导航项高亮和下划线
    function initFirstTab() {
        tabItems[0].classList.add('active');
        const width = tabItems[0].offsetWidth;
        const left = tabItems[0].offsetLeft;
        underline.style.width = `${width}px`;
        underline.style.left = `${left}px`;
    }

    // 弹性缓动函数（类似 CSS 的 cubic-bezier(0.68, -0.55, 0.27, 1.55)）
    function easeOutElastic(t) {
        const p = 0.3;
        const s = p / 4;
        return t === 0
            ? 0
            : t === 1
                ? 1
                : Math.pow(2, -10 * t) * Math.sin((t - s) * (2 * Math.PI) / p) + 1;
    }

    // 自定义滚动函数（带弹性效果）
    function smoothScrollTo(targetY, duration = 800) {
        const startY = window.scrollY;
        const diff = targetY - startY;
        let startTime = null;

        function animateScroll(currentTime) {
            if (!startTime) startTime = currentTime;
            const elapsed = currentTime - startTime;
            const progress = Math.min(elapsed / duration, 1);
            const easeProgress = easeOutElastic(progress); // 应用弹性缓动

            window.scrollTo(0, startY + diff * easeProgress);

            if (progress < 1) {
                requestAnimationFrame(animateScroll);
            }
        }

        requestAnimationFrame(animateScroll);
    }

    // 点击导航项时，弹性滚动到目标 div
    tabItems.forEach(item => {
        item.addEventListener('click', function () {
            // 移除所有 active 类
            tabItems.forEach(i => i.classList.remove('active'));

            // 当前点击的项添加 active 类
            this.classList.add('active');
            const targetId = this.getAttribute('data-target');
            const targetSection = document.getElementById(targetId);

            // 计算目标位置（考虑导航栏高度）
            const navHeight = document.querySelector('.navbar').offsetHeight;
            const targetY = targetSection.offsetTop - navHeight;

            // 弹性滚动到目标位置
            smoothScrollTo(targetY);

            // 动态调整下划线位置
            const width = this.offsetWidth;
            const left = this.offsetLeft;
            underline.style.width = `${width}px`;
            underline.style.left = `${left}px`;
        });
    });

    // 初始化
    initFirstTab();
});