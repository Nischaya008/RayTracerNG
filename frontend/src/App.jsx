import { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
import { Link } from 'react-scroll';
import { FaGithub, FaLinkedin, FaShareAlt } from 'react-icons/fa';

// Import components
import Features from './components/Features';
import TechStack from './components/TechStack';
import Architecture from './components/Architecture';
import Gallery from './components/Gallery';
import Download from './components/Download';
import Contribute from './components/Contribute';
import SocialLinksModal from './components/SocialLinksModal';
import AboutModal from './components/AboutModal';

const App = () => {
  const [isScrolled, setIsScrolled] = useState(false);
  const [showSocialLinks, setShowSocialLinks] = useState(false);
  const [showAboutModal, setShowAboutModal] = useState(false);

  useEffect(() => {
    const handleScroll = () => {
      setIsScrolled(window.scrollY > 50);
    };

    window.addEventListener('scroll', handleScroll);

    return () => {
      window.removeEventListener('scroll', handleScroll);
    };
  }, []);

  return (
    <div className="min-h-screen bg-primary">
      {/* Navigation */}
      <nav className={`fixed w-full z-50 transition-all duration-300 ${
        isScrolled ? 'top-0 py-1' : 'top-4 py-4'
      }`}>
        <div className={`container mx-auto px-8 flex justify-between items-center ${
          isScrolled ? 'glass backdrop-blur-xl bg-gradient-to-r from-primary/10 to-secondary/10 border-b border-white/5 shadow-2xl rounded-b-2xl mx-4 py-3' : ''
        }`}>
          <motion.h1 
            className="text-2xl font-display font-bold neon-text pl-4"
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            transition={{ duration: 0.5 }}
          >
            RayTracerNG
          </motion.h1>
          
          <div className="hidden md:flex space-x-5 pr-4">
            {['Features', 'Tech Stack', 'Architecture', 'Gallery', 'Download', 'Contribute'].map((item) => (
              <Link
                key={item}
                to={item.toLowerCase()}
                smooth={true}
                duration={500}
                className="text-white/70 hover:text-accent cursor-pointer transition-all duration-300 relative group px-2 py-1"
              >
                {item}
                <span className="absolute bottom-0 left-0 w-0 h-0.5 bg-accent/50 transition-all duration-300 group-hover:w-full"></span>
              </Link>
            ))}
          </div>
        </div>
      </nav>

      {/* Hero Section */}
      <section className="min-h-screen flex items-center justify-center relative overflow-hidden">
        <div className="absolute inset-0 bg-gradient-to-b from-primary to-secondary opacity-50" />
        <div className="container relative z-10 text-center">
          <motion.h1
            className="text-6xl md:text-8xl font-display font-bold neon-text mb-6"
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.8 }}
          >
            RayTracerNG
          </motion.h1>
          <motion.p
            className="text-xl md:text-2xl text-gray-300 mb-12"
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.8, delay: 0.2 }}
          >
            Real-time 2D Ray Tracing Engine with Modern OpenGL
          </motion.p>
          <motion.div
            className="flex flex-col md:flex-row gap-4 justify-center"
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.8, delay: 0.4 }}
          >
            <a href="https://github.com/Nischaya008/RayTracerNG" className="btn btn-primary" target="_blank" rel="noopener noreferrer">
              View on GitHub
            </a>
            <a href="#download" className="btn btn-secondary">
              Download Installer
            </a>
          </motion.div>
        </div>
      </section>

      {/* Main Content */}
      <main>
        <Features />
        <TechStack />
        <Architecture />
        <Gallery />
        <Download />
        <Contribute />
      </main>

      {/* Footer */}
      <footer className="bg-secondary py-8">
        <div className="container mx-auto px-4">
          <div className="text-center">
            <div className="text-gray-400 text-sm">
              © 2025 RayTracerNG. All rights reserved.
            </div>
          </div>
        </div>
      </footer>

      {/* Social Links */}
      <SocialLinksModal onAboutClick={() => setShowAboutModal(true)} />

      {/* About Modal */}
      <AnimatePresence>
        {showAboutModal && (
          <AboutModal onClose={() => setShowAboutModal(false)} />
        )}
      </AnimatePresence>
    </div>
  );
};

export default App;
