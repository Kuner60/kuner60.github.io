// product-center.js - 产品中心页面JavaScript

// 页面加载完成后执行
document.addEventListener('DOMContentLoaded', function () {
    // 更新导航栏当前页标识
    updateNavActiveState();

    // 初始化模块卡片交互
    initModuleCards();

    // 初始化悬浮导航窗
    initFloatingNav();

    // 初始化返回首页按钮
    initHomeButton();

    // 初始化模拟数据更新
    initDataSimulation();

    // 初始化移动端菜单
    initMobileMenu();
});

// 更新导航栏当前页标识
function updateNavActiveState() {
    const currentPage = window.location.pathname;
    const navLinks = document.querySelectorAll('.nav-links a');

    navLinks.forEach(link => {
        if (link.getAttribute('href') === 'product-center.html') {
            link.classList.add('active');
        } else {
            link.classList.remove('active');
        }
    });
}

// 初始化模块卡片交互
function initModuleCards() {
    const moduleCards = document.querySelectorAll('.module-card, .module-card-large');

    moduleCards.forEach(card => {
        // 鼠标悬停效果
        card.addEventListener('mouseenter', function () {
            this.style.transform = 'translateY(-8px)';
            this.style.boxShadow = '0 15px 30px rgba(0, 0, 0, 0.3)';
        });

        card.addEventListener('mouseleave', function () {
            this.style.transform = 'translateY(0)';
            this.style.boxShadow = '';
        });

        // 点击效果
        card.addEventListener('click', function () {
            this.style.transform = 'scale(0.98)';
            setTimeout(() => {
                this.style.transform = 'scale(1)';
            }, 150);
        });
    });
}

// 初始化悬浮导航窗
function initFloatingNav() {
    const floatingNavBtns = document.querySelectorAll('.floating-nav-btn');

    floatingNavBtns.forEach(btn => {
        btn.addEventListener('click', function (e) {
            e.preventDefault();

            const targetId = this.getAttribute('data-target');
            const targetElement = document.getElementById(targetId);

            if (targetElement) {
                // 平滑滚动到目标位置
                window.scrollTo({
                    top: targetElement.offsetTop - 100,
                    behavior: 'smooth'
                });

                // 添加果冻动画效果
                this.classList.add('jelly-effect');

                // 移除动画类以便下次点击时可以再次触发
                setTimeout(() => {
                    this.classList.remove('jelly-effect');
                }, 500);

                // 高亮对应的部分
                highlightSection(targetElement);
            }
        });
    });

    // 监听滚动，显示/隐藏悬浮窗
    let lastScrollTop = 0;
    const floatingNav = document.querySelector('.floating-nav');

    if (floatingNav) {
        window.addEventListener('scroll', function () {
            const scrollTop = window.pageYOffset || document.documentElement.scrollTop;

            // 当滚动超过200px时显示悬浮窗
            if (scrollTop > 200) {
                floatingNav.style.opacity = '1';
                floatingNav.style.transform = 'translateX(0)';
            } else {
                floatingNav.style.opacity = '0';
                floatingNav.style.transform = 'translateX(20px)';
            }

            lastScrollTop = scrollTop;
        });

        // 页面加载时初始化悬浮窗位置和透明度
        setTimeout(() => {
            floatingNav.style.opacity = '0';
            floatingNav.style.transform = 'translateX(20px)';
            floatingNav.style.transition = 'opacity 0.5s, transform 0.5s';

            // 延迟显示悬浮窗
            setTimeout(() => {
                floatingNav.style.opacity = '1';
                floatingNav.style.transform = 'translateX(0)';
            }, 1000);
        }, 100);
    }
}

// 高亮对应的部分
function highlightSection(targetElement) {
    // 移除所有高亮
    const sections = document.querySelectorAll('.product-modules, .tech-specs, .system-advantages');
    sections.forEach(section => {
        section.classList.remove('highlight-section');
    });

    // 添加高亮
    if (targetElement) {
        targetElement.classList.add('highlight-section');

        // 3秒后移除高亮
        setTimeout(() => {
            targetElement.classList.remove('highlight-section');
        }, 3000);
    }
}

// 初始化返回首页按钮
function initHomeButton() {
    const homeBtn = document.querySelector('.floating-home-btn');

    if (homeBtn) {
        homeBtn.addEventListener('click', function (e) {
            // 添加点击动画
            this.style.transform = 'scale(0.9)';
            setTimeout(() => {
                this.style.transform = 'scale(1)';
            }, 150);
        });
    }
}

// 初始化模拟数据更新
function initDataSimulation() {
    // 模拟数据更新（仅用于演示）
    function updateDemoData() {
        const dataValues = document.querySelectorAll('.data-value');
        dataValues.forEach(value => {
            if (value.textContent === '8888') return; // 跳过光照值

            const currentValue = parseFloat(value.textContent);
            const change = (Math.random() - 0.5) * 0.3; // 随机变化
            const newValue = Math.max(0, currentValue + change);

            // 简单动画效果
            value.classList.remove('updated');
            void value.offsetWidth; // 触发重绘
            value.textContent = newValue.toFixed(1);
            value.classList.add('updated');
        });
    }

    // 每8秒更新一次模拟数据
    setInterval(updateDemoData, 8000);
}

// 初始化移动端菜单
function initMobileMenu() {
    const mobileMenuBtn = document.getElementById('mobileMenuBtn');
    const navLinksContainer = document.getElementById('navLinks');

    if (mobileMenuBtn && navLinksContainer) {
        mobileMenuBtn.addEventListener('click', () => {
            navLinksContainer.classList.toggle('active');
        });

        // 点击导航链接后关闭移动端菜单
        const navItems = document.querySelectorAll('.nav-links a');
        navItems.forEach(item => {
            item.addEventListener('click', () => {
                navLinksContainer.classList.remove('active');
            });
        });
    }
}

// 平滑滚动到锚点（用于页面内链接）
document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function (e) {
        e.preventDefault();

        const targetId = this.getAttribute('href');
        if (targetId === '#') return;

        const targetElement = document.querySelector(targetId);
        if (targetElement) {
            window.scrollTo({
                top: targetElement.offsetTop - 80,
                behavior: 'smooth'
            });
        }
    });
});

// 页面加载动画
window.addEventListener('load', function () {
    // 延迟触发卡片动画
    setTimeout(() => {
        const cards = document.querySelectorAll('.module-card, .module-card-large, .advantage-item');
        cards.forEach(card => {
            card.style.animationPlayState = 'running';
        });
    }, 300);

    // 初始化数据模拟
    setTimeout(() => {
        const dataValues = document.querySelectorAll('.data-value');
        dataValues.forEach(value => {
            if (value.textContent !== '8888') {
                value.classList.add('updated');
                setTimeout(() => {
                    value.classList.remove('updated');
                }, 500);
            }
        });
    }, 1000);
});